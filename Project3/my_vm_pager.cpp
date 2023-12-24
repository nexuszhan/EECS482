#include "vm_pager.h"
#include "my_utils.h"

#include <cstring>
#include <string>
#include <algorithm>

#include <assert.h>
#include <iostream>
using std::cout;
using std::endl;

using std::string;

vector<physical_page> physical_table; 
queue<unsigned int> available_physical_pages; 
deque<unsigned int> clock_queue; 

map<file_id, unsigned int> peer_in_phys; 
map<file_id, set<virtual_page *>> peer_map; 

unordered_map<pid_t, shared_ptr<page_table>> tables;
shared_ptr<page_table> cur_table;

queue<unsigned int> available_swap_blocks;

unsigned int num_memory_pages; 
unsigned int num_swap_blocks; // number of swap blocks that can be reserved 
pid_t cur_pid = -1;

void vm_init(unsigned int memory_pages, unsigned int swap_blocks)
{
    num_memory_pages = memory_pages;
    num_swap_blocks = swap_blocks;

    // pin a zero page in physical memory
    std::memset(vm_physmem, 0, VM_PAGESIZE);
    
    // initialize physical_table
    physical_table.resize(num_memory_pages);

    for (unsigned int i=1; i<num_memory_pages; i++)
        available_physical_pages.push(i);

    for (unsigned int i=0; i<num_swap_blocks; i++)
        available_swap_blocks.push(i);
}

int vm_create(pid_t parent_pid, pid_t child_pid)
{
    // eager reservation
    if (tables.find(parent_pid) != tables.end())
    {
        if (tables[parent_pid]->occupied_swap_blocks > num_swap_blocks)
            return -1;
        num_swap_blocks -= tables[parent_pid]->occupied_swap_blocks;
        // more things needs to be done for advanced version
    }
    
    shared_ptr<page_table> new_table(new page_table{});
    tables[child_pid] = new_table;
    return 0;
}

void vm_switch(pid_t pid)
{
    if (cur_pid!=pid)
    {
        cur_table = tables[pid];
        page_table_base_register = cur_table->ptbr;
        cur_pid = pid;
    }
}

int vm_fault(const void* addr, bool write_flag)
{
    unsigned int vpn = (reinterpret_cast<uintptr_t>(addr) - reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR)) >> 16;
    if (vpn >= cur_table->cnt)
        return -1;

    virtual_page* cur_page = cur_table->vps[vpn];
    // check whether there is peer in physical memory
    // if (!cur_page->_id._swap_backed && !cur_page->_resident && peer_in_phys.find(cur_page->_id) != peer_in_phys.end())
    // {
    //     cur_page->_entry->ppage = peer_in_phys[cur_page->_id];
    //     cur_page->_resident = true;
    // }
    if (write_flag) // write fault
    {
        // if ((!cur_page->_resident && cur_page->_id._swap_backed) || cur_page->_entry->ppage == 0 ||
        //     (!cur_page->_id._swap_backed && files.find(cur_page->_id) == files.end()))
        if (!cur_page->check_residence() || cur_page->_entry->ppage == 0)
        {
            get_free_physical_page();
            if (set_physical_page(cur_page) == -1)
                return -1;
        }
        // change dirty and write_enable
        physical_table[cur_page->_entry->ppage].dirty = true;
        cur_page->_entry->write_enable = 1;
    }
    else // read fault
    {
        // if ((!cur_page->_resident && cur_page->_id._swap_backed) ||
        //     (!cur_page->_id._swap_backed && files.find(cur_page->_id) == files.end()))
        if (!cur_page->check_residence())
        {
            get_free_physical_page();
            if (set_physical_page(cur_page) == -1)
                return -1;
        }
        else if (physical_table[cur_page->_entry->ppage].dirty)
        {
            cur_page->_entry->write_enable = 1;
        }
    }
    // change read_enable and referenced 
    cur_page->_entry->read_enable = 1;
    physical_table[cur_page->_entry->ppage].referenced = true;

    set_peer(peer_map[cur_page->_id], cur_page);

    return 0;
}

void vm_destroy()
{   
    // clean up occupied physical pages and swap blocks 
    for (virtual_page* vp : cur_table->vps)
    {
        peer_map[vp->_id].erase(vp);
        // if (vp->_resident && vp->_entry->ppage != 0)
        if (vp->check_residence() && vp->_entry->ppage != 0)
        {
            if (vp->_id._swap_backed)
            {
                // reset dirty bit 
                physical_table[vp->_entry->ppage].dirty = false;
                available_physical_pages.push(vp->_entry->ppage);
                // remove the physical page from clock queue
                auto it = std::find(clock_queue.begin(), clock_queue.end(), vp->_entry->ppage);
                if (it != clock_queue.end())
                    clock_queue.erase(it);
                peer_in_phys.erase(vp->_id);
            }
        }
        if (vp->_id._swap_backed)
            available_swap_blocks.push(vp->_id._block);
    }
    
    num_swap_blocks += cur_table->occupied_swap_blocks;
   
    tables.erase(cur_pid);
    cur_table.reset();
}

void* vm_map(const char* filename, unsigned int block)
{
    if (cur_table->cnt >= MAX_PAGE_NUM)
        return nullptr;
    
    uintptr_t new_addr = reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR) + cur_table->cnt * VM_PAGESIZE;
    if (!filename) // swap-backed pages
    {
        // check if there are enough swap blocks
        if (!num_swap_blocks)
            return nullptr; 
        
        page_table_entry_t* cur_entry = page_table_base_register + cur_table->cnt; 
        // cur_entry->ppage = 0;
        // cur_entry->read_enable = 1;
        // cur_entry->write_enable = 0;
        *cur_entry = { .ppage = 0, .read_enable = 1, .write_enable = 0 };

        // reserve a block number to the new page
        unsigned int swap_block = available_swap_blocks.front();
        available_swap_blocks.pop();
        // virtual_page* new_page = new virtual_page(true, true, cur_entry, swap_block);
        virtual_page* new_page = new virtual_page(true, cur_entry, swap_block);
        cur_table->vps.emplace_back(new_page);

        num_swap_blocks--;
        cur_table->occupied_swap_blocks++;
        
        peer_map[new_page->_id].insert(new_page);
        peer_in_phys[new_page->_id] = 0;
    }
    else // file-backed pages
    {
        int temp = 0;
        string temp_filename="";
        bool end_flag=false;

        while (!end_flag)
        {
            unsigned int vpn = (reinterpret_cast<uintptr_t>(filename + temp) - reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR)) >> 16;
            unsigned int offset = (reinterpret_cast<uintptr_t>(filename + temp) - reinterpret_cast<uintptr_t>(VM_ARENA_BASEADDR))-(vpn<<16);
            if (vpn >= cur_table->cnt)
                return nullptr; // end of the valid zone
            
            if (!cur_table->vps[vpn]->_entry->read_enable)
            {
                if (vm_fault(filename + temp, false) == -1)
                    return nullptr;
            }
            
            char *start = static_cast<char *>(vm_physmem);
            char tmp = start[cur_table->vps[vpn]->_entry->ppage*VM_PAGESIZE + offset];
            if (tmp == '\0')
                end_flag=true;
            
            temp_filename += tmp; // do not need to varify if the name is legal here
            temp++;
        }
        page_table_entry_t* cur_entry = page_table_base_register + cur_table->cnt; 
        // virtual_page *new_page = new virtual_page(false, false, cur_entry, block);
        virtual_page *new_page = new virtual_page(false, cur_entry, block);
        new_page->_id._filename = temp_filename; 
        // cur_entry->read_enable = cur_entry->write_enable = 0;
        // cur_entry->ppage = num_memory_pages+1; // don't care actually
        *cur_entry = { .ppage = num_memory_pages, .read_enable = 0, .write_enable = 0 };

        // if (peer_in_phys.find(new_page->_id) != peer_in_phys.end())
        if (new_page->check_residence())
        {
            // If the file_id is already in physmem, directly map the new page to the physical page
            cur_entry->ppage = peer_in_phys[new_page->_id];
           
            // there may be no existing peer in physical table
            // so deduce the protection bits by referenced and dirty bits
            if (physical_table[peer_in_phys[new_page->_id]].referenced)
            {
                cur_entry->read_enable = 1;
                if (physical_table[peer_in_phys[new_page->_id]].dirty)
                    cur_entry->write_enable = 1;
            }
            // new_page->_resident = true;
        }
      
        peer_map[new_page->_id].insert(new_page);
        cur_table->vps.emplace_back(new_page);
    }
    cur_table->cnt++;

    return (void*)(new_addr);
}