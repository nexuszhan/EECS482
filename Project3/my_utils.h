/* Helper classes and functions */
#pragma once

#include "vm_pager.h"

#include <vector>
#include <map>
#include <queue>
#include <cstring>
#include <string>
#include <set>
#include <deque>
#include <unordered_map>
#include <memory>

using std::queue;
using std::set;
using std::string;
using std::vector;
using std::map;
using std::deque;
using std::shared_ptr;
using std::unordered_map;
// there can be at most 256 pages in an arena
const unsigned int MAX_PAGE_NUM = VM_ARENA_SIZE / VM_PAGESIZE;

class file_id
{
    public:
        string _filename; 
        unsigned int _block; 
        bool _swap_backed;
        
        file_id() : _block(0), _swap_backed(true) {}

        file_id & operator=(const file_id & id)
        {
            _filename = id._filename;
            _block = id._block;
            _swap_backed = id._swap_backed;
            return *this;
        }

        bool operator<(const file_id &id) const
        {
            if (_swap_backed && id._swap_backed)
                return _block < id._block;
            if (!_swap_backed && !id._swap_backed)
            {
                if (_filename!=id._filename)
                    return _filename < id._filename;
                else
                    return _block < id._block;
            }
            if (_swap_backed && !id._swap_backed)
                return false;
            return true;
        }
};

extern map<file_id, unsigned int> peer_in_phys; // all peer in physical table

class virtual_page
{
    public:
        // bool _resident; // whether in physical table
        page_table_entry_t* _entry; 
        file_id _id;

        // virtual_page(bool resident, bool swap_backed, page_table_entry_t* entry, unsigned int block) : 
            // _resident(resident), _entry(entry) { _id._swap_backed = swap_backed; _id._block = block; }
        virtual_page(bool swap_backed, page_table_entry_t* entry, unsigned int block) : 
            _entry(entry) { _id._swap_backed = swap_backed; _id._block = block; }
        virtual ~virtual_page() {}

        // return true if is in physical table
        bool check_residence()
        {
            return (peer_in_phys.find(_id) != peer_in_phys.end());
            // return (_id._swap_backed && _resident) || (!_id._swap_backed && peer_in_phys.find(_id) != peer_in_phys.end());
        }
};

class page_table
{
    public:
        page_table_entry_t* ptbr;
        vector<virtual_page*> vps;
        unsigned int cnt; // number of allocated pages in arena
        unsigned int occupied_swap_blocks;

        page_table() : ptbr(new page_table_entry_t[MAX_PAGE_NUM]), cnt(0)
            , occupied_swap_blocks(0) 
        {
            for (unsigned int i=0; i<MAX_PAGE_NUM; i++)
            {
                (ptbr+i)->read_enable = (ptbr+i)->write_enable = 0;
            }
        }
        ~page_table() 
        { 
            for (unsigned int i=0; i<cnt; i++)
                delete vps[i];
            delete [] ptbr; 
        }
};

class physical_page
{
    public:
        bool dirty, referenced;
        file_id id;

        physical_page() : dirty(false), referenced(false) {}
        ~physical_page() {} ;
};

extern vector<physical_page> physical_table; 
extern queue<unsigned int> available_physical_pages; // ppage(s) that are free
extern deque<unsigned int> clock_queue; // used for clock algorithm

extern map<file_id, set<virtual_page *>> peer_map; // all virtual pages with same file_id

extern unordered_map<pid_t, shared_ptr<page_table>> tables; // map process ID to the process's page table
extern shared_ptr<page_table> cur_table; // current process's page table

extern queue<unsigned int> available_swap_blocks;

// use clock algorithm to evict a physical page if there is no free one now
void get_free_physical_page();

// set up a free physical page
int set_physical_page(virtual_page* cur_page);

// set all virtual pages with the same file_id 
void set_peer(set<virtual_page*> & peer, virtual_page* cur_page);