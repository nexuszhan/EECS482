#include "my_utils.h"

void get_free_physical_page()
{
    while (available_physical_pages.empty()) // no free physical page
    {
        unsigned int ppage = clock_queue.front();
        clock_queue.pop_front();

        file_id & id = physical_table[ppage].id;
        set<virtual_page*> & victims = peer_map[id];
        if (!physical_table[ppage].referenced)
        {
            // evict the page(s)
            // for (virtual_page* victim : victims)
            //     victim->_resident=false;
            if (physical_table[ppage].dirty)
            {
                char *start = static_cast<char *>(vm_physmem);
                if (id._swap_backed)
                    file_write(nullptr, id._block, (void *)(start + ppage * VM_PAGESIZE));
                else
                    file_write(id._filename.c_str(), id._block, (void *)(start + ppage * VM_PAGESIZE));
            } 
            peer_in_phys.erase(id);

            // now this physical page is available
            physical_table[ppage].dirty = false;
            available_physical_pages.push(ppage);
            break;
        }
        
        for (virtual_page* victim : victims)
            victim->_entry->write_enable = victim->_entry->read_enable = 0;

        physical_table[ppage].referenced = false;
        clock_queue.push_back(ppage);
    }
}

int set_physical_page(virtual_page* cur_page)
{
    unsigned int ppage = available_physical_pages.front();
    
    char* start = static_cast<char *>(vm_physmem);
    // if (!cur_page->_resident)
    if (!cur_page->check_residence())
    {
        // read from file if non-resident
        if (cur_page->_id._swap_backed)
            file_read(nullptr, cur_page->_id._block, (void *)(start + ppage * VM_PAGESIZE));
        else
        { 
            if (file_read(cur_page->_id._filename.c_str(), cur_page->_id._block, (void *)(start + ppage * VM_PAGESIZE)) == -1)
                return -1;
            // peer_in_phys[cur_page->_id] = ppage;
        } 
    }
    else // the page is currently mapped to pinned memory
    {
        std::memcpy((void *)(start + ppage * VM_PAGESIZE), (void *)(start + (cur_page->_entry->ppage) * VM_PAGESIZE), VM_PAGESIZE);
    }
    // physical_table[ppage].id._block = cur_page->_id._block;
    // physical_table[ppage].id._filename = cur_page->_id._filename;
    // physical_table[ppage].id._swap_backed = cur_page->_id._swap_backed;
    physical_table[ppage].id = cur_page->_id;

    available_physical_pages.pop();
    clock_queue.push_back(ppage);

    // cur_page->_resident = true;
    peer_in_phys[cur_page->_id] = ppage;
    cur_page->_entry->ppage = ppage;

    return 0;
}

void set_peer(set<virtual_page*> & peer, virtual_page* cur_page)
{
    for (virtual_page* vp : peer)
    {
        // vp->_resident = true;
        *vp->_entry = { .ppage = cur_page->_entry->ppage, .read_enable = 1, 
                        .write_enable = cur_page->_entry->write_enable};
        // vp->_entry->read_enable = 1;
        // vp->_entry->write_enable = cur_page->_entry->write_enable;
        // vp->_entry->ppage = cur_page->_entry->ppage;
    }
}