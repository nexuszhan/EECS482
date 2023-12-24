#include "utils.h"
#include "fs_server.h"

#include <vector>
#include <sstream>
#include <queue>
#include <array>
#include <string>
#include <iostream>
#include <unordered_map>

using std::queue;
using std::array;
using std::string;
using std::getline;
using std::vector;
using std::istringstream;
using std::unordered_map;

struct target_inode{
    uint32_t inode_block=0;
    bool valid=false;
    uint32_t first_empty_block = FS_MAXFILEBLOCKS + 1;
    unsigned int first_empty_entry_index = 0;
    uint32_t parent_inode_block=0;
};

class FileServer
{
    
    private:
        int port;
        int max_queue_size;
        int sockfd;
        // unsigned int MAX_MESSAGE_SIZE;
        
        queue<unsigned int> free_blocks;
        const unsigned int max_header_len = 13+FS_MAXUSERNAME+FS_MAXPATHNAME+3+3; //157

        boost::mutex free_block_lock; // protect free_blocks
        vector<boost::shared_mutex*> disk_locks; // protect disk data

    public:
        FileServer(int port) : port(port), max_queue_size(30), sockfd(0) 
        {
            disk_locks.reserve(FS_DISKSIZE);
            for (unsigned int i=0; i<FS_DISKSIZE; i++)
            {
                disk_locks.push_back(new boost::shared_mutex());
            }
        }

        ~FileServer() {}

        const int get_sockfd() { return sockfd; };

        void init_free_blocks();

        void init_server();

        bool path_parser(vector<string> &, const string &);

        target_inode traverse_dir(const string &, vector<string> &);

        void check_existance(target_inode&, fs_inode &, const string&, fs_direntry *);

        void return_empty_block(unsigned int block);

        void handle_create(istringstream&, const string&, const string&, int);

        void handle_delete(const string&, const string&, int);

        void handle_readblock(istringstream&, const string&, const string&, int);

        void handle_writeblock(istringstream&, const string&, const string&, int);

        int check_request_type(const string&, const string&);

        void handle_requests(int);
};