#include "file_server.h"

#include <set>
#include <iostream>

using std::set;
using std::stoi;
using std::cout;
using std::endl;

void FileServer::init_free_blocks()
{
    // initilize free blocks
    set<unsigned int> all_blocks;
    for (unsigned int i=1; i<FS_DISKSIZE; i++)
        all_blocks.insert(i);
    
    // traverse the initial file system (BFS)
    fs_inode inode;
    disk_readblock(0, &inode);
    queue<unsigned int> dir_inodes;
    for (unsigned int i=0; i<inode.size; i++)
        dir_inodes.push(inode.blocks[i]);
    
    while (!dir_inodes.empty())
    {
        uint32_t dir_block = dir_inodes.front();
        dir_inodes.pop();
        all_blocks.erase(dir_block);
        
        fs_direntry entries[FS_DIRENTRIES];
        disk_readblock(dir_block, &entries);
        
        // check all entries
        for (unsigned int i=0; i<FS_DIRENTRIES; i++)
        {
            fs_direntry& entry = entries[i];
            if (!entry.inode_block) // inode_block=0 means unused
                continue;

            all_blocks.erase(entry.inode_block);
            disk_readblock(entry.inode_block, &inode);

            if (inode.type == 'f')
            {
                for (unsigned int j=0; j<inode.size; j++)
                    all_blocks.erase(inode.blocks[j]);
            }
            else if (inode.type == 'd')
            {
                for (unsigned int j=0; j<inode.size; j++)
                    dir_inodes.push(inode.blocks[j]);
            }
        }
    }
    
    for (unsigned int block : all_blocks)
        free_blocks.push(block);
}

void FileServer::init_server()
{
    // initialize free blocks
    init_free_blocks();
    // set up socket
    sockfd = setup_socket(port, max_queue_size);
    print_port(port);
}

bool FileServer::path_parser(vector<string> &path_names, const string &pathname){
    // parse the pathname and store the result in path_names
    // return true on success, return false otherwise
    // use BFS to traverse file system
    istringstream path_stream(pathname);
    string tmp;
    // must start with /
    if (pathname[0] != '/' || pathname[pathname.length()-1] == '/')// pathname[pathname.length()-1] == '/'
        return false;
    getline(path_stream, tmp, '/');
    while (getline(path_stream, tmp, '/'))
    {
        if (tmp.length() > FS_MAXFILENAME || tmp.length()==0)
            return false;
        path_names.push_back(tmp);
    }
    return true;
}

target_inode FileServer::traverse_dir(const string &username, vector<string> &path_names)
{
    // traverse the path_names to get the last inode's inode_block (check validity while traversing)
    // only traverse all the dir and return the inode_block for the target dir
    target_inode result; // = {0, true, FS_MAXFILEBLOCKS + 1, 0};
    result.valid=true;
    fs_inode inode;
    unsigned int dir_block=0;
    if (!path_names.empty())
    {
        disk_locks[dir_block]->lock_shared();
        disk_readblock(dir_block, &inode);
    }
        
    for (auto itr=path_names.begin(); itr!=path_names.end(); itr++){ // type of itr
        check_existance(result, inode, *itr, nullptr);
        auto temp_itr = itr;
        temp_itr++;
        if(!result.valid || temp_itr == path_names.end()){ // do not readblock for last dir
            return result;
        }
        disk_locks[result.inode_block]->lock_shared();
        disk_locks[dir_block]->unlock_shared();
        disk_readblock(result.inode_block, &inode);
        result.parent_inode_block = dir_block = result.inode_block;

        if (string(inode.owner) != username || inode.type != 'd')
        {
            result.valid=false;
            return result;
        }
    }
    return result;
}

void FileServer::check_existance(target_inode& result, fs_inode &inode, const string& target_name, fs_direntry *output_entries)
{
    // iterate through a dir to see if a certain file/dir name exist
    // return inode_block if exists
    result.valid = false;
    fs_direntry entries[FS_DIRENTRIES];
    for (unsigned int i = 0; i < inode.size; i++)
    {
        unsigned int block = inode.blocks[i];
        
        disk_readblock(block, &entries);
        for (unsigned int j = 0; j < FS_DIRENTRIES; j++){
            fs_direntry &entry = entries[j];
            if (entry.inode_block && string(entry.name) == target_name){
                result.inode_block = entry.inode_block;
                result.valid=true;
                result.first_empty_block = i;
                result.first_empty_entry_index = j;
                if (output_entries)
                    memcpy(output_entries, entries, sizeof(entries));
                return;
            } else if (output_entries!=nullptr && !entry.inode_block && result.first_empty_block == (FS_MAXFILEBLOCKS + 1) ){
                result.first_empty_block = block;
                result.first_empty_entry_index = j;
                if (output_entries)
                    memcpy(output_entries, entries, sizeof(entries));
            }
        }
    }
    return;
}

void FileServer::return_empty_block(unsigned int block)
{
    // return the unused empty block
    boost::lock_guard<boost::mutex> lock(free_block_lock);
    free_blocks.push(block);
}

void FileServer::handle_create(istringstream &req, const string &username, const string &pathname, int connectionfd)
{
    string type;
    if (req >> type)
    {
        // check type
        if (type != "f" && type != "d")
            return;
        // check username and pathname valid or not
        vector<string> path_names;
        if (!path_parser(path_names, pathname))
            return;
        
        // check and reserve an empty block
        unsigned int new_inode_block;
        {
        boost::lock_guard<boost::mutex> lock(free_block_lock);
        if (free_blocks.empty()) return; // space in disk
        new_inode_block = free_blocks.front();
        free_blocks.pop();
        }

        string new_file_name = path_names[path_names.size() - 1];
        path_names.pop_back();
        target_inode t_inode;
        t_inode = traverse_dir(username, path_names); // last dir inode
        if (!t_inode.valid){
            disk_locks[t_inode.parent_inode_block]->unlock_shared();
            return_empty_block(new_inode_block);
            return;
        }
        fs_inode parent_inode;
        {
        boost::unique_lock<boost::shared_mutex> write_lock(*disk_locks[t_inode.inode_block]);
        if (t_inode.inode_block)
        {
            disk_locks[t_inode.parent_inode_block]->unlock_shared();
        }
            
        disk_readblock(t_inode.inode_block, &parent_inode);
        if ((string(parent_inode.owner) != username && string(parent_inode.owner)!="") || parent_inode.type != 'd'){
            return_empty_block(new_inode_block);
            return;
        }
        // check type & set target
        // check if the file already exists, or same name with dir
        fs_direntry parent_entries[FS_DIRENTRIES];
        target_inode parent_node_info;
        check_existance(parent_node_info, parent_inode, new_file_name, parent_entries);
        if (parent_node_info.valid){
            return_empty_block(new_inode_block);
            return;
        }

        unsigned int new_entry = parent_node_info.first_empty_entry_index;
        unsigned int dir_block = parent_node_info.first_empty_block;

        // check if there is enough space in disk and dir
        bool change_parent = false;
        // unsigned int new_inode_block;
        {
        boost::lock_guard<boost::mutex> lock(free_block_lock);
        if ( dir_block == FS_MAXFILEBLOCKS + 1 ) 
        { // no empty entry in cur blocks            
            if (free_blocks.empty() || parent_inode.size + 1 > FS_MAXFILEBLOCKS){
                free_blocks.push(new_inode_block);
                return; 
            }
            dir_block = free_blocks.front();
            free_blocks.pop();

            parent_inode.blocks[parent_inode.size] = dir_block;
            parent_inode.size++;
            change_parent = true;
        }
        }
        // add an fs_direntry to the dir inode
        if (change_parent) // for a new direntry, initialize its inode_blocks to 0
        {
            for (unsigned int i=0; i<FS_DIRENTRIES; i++)
                parent_entries[i].inode_block = 0;
        }
        // new_file_name length < 60?
        strcpy(parent_entries[new_entry].name, new_file_name.c_str());
        parent_entries[new_entry].inode_block = new_inode_block;

        // create an fs_inode
        fs_inode new_inode;
        new_inode.type = type[0];
        strcpy(new_inode.owner, username.c_str());
        new_inode.size = 0;
        disk_writeblock(new_inode_block, &new_inode);
        disk_writeblock(dir_block, &parent_entries);
        if (change_parent)
        {
            disk_writeblock(t_inode.inode_block, &parent_inode);
        }
        }
        // send response
        string message = "FS_CREATE "+username+" "+pathname+" "+type+'\0';
        send_response(message, connectionfd);
    }
}

void FileServer::handle_delete(const string &username, const string &pathname, int connectionfd)
{
    // if a block is empty, move it to free_blocks
    // check username and pathname valid or not
    vector<string> path_names;
    if (!path_parser(path_names, pathname))
        return;
    string target_name = path_names[path_names.size() - 1];
    path_names.pop_back();
    target_inode t_inode = traverse_dir(username, path_names); // last inode
    if (!t_inode.valid){
        disk_locks[t_inode.parent_inode_block]->unlock_shared();
        return;
    }
    fs_inode parent_inode;
    unsigned int parent_entry_inode_block;
    fs_inode file_inode;
    { // write lock (t_inode)
    boost::unique_lock<boost::shared_mutex> write_lock(*disk_locks[t_inode.inode_block]);
    if (t_inode.inode_block)
    {
        disk_locks[t_inode.parent_inode_block]->unlock_shared();
    }
        
    disk_readblock(t_inode.inode_block, &parent_inode);
    if ((string(parent_inode.owner) != username && string(parent_inode.owner) != "") || parent_inode.type != 'd')
        return;
    // check if the file/dir exists,
    fs_direntry parent_entries[FS_DIRENTRIES];
    target_inode parent_node_info;
    check_existance(parent_node_info, parent_inode, target_name, parent_entries);
    if (!parent_node_info.valid)
        return;

    // read file_inode (file or dir)
    { // write lock (file_inode)
    boost::unique_lock<boost::shared_mutex> write_child_lock(*disk_locks[parent_node_info.inode_block]);
    disk_readblock(parent_node_info.inode_block, &file_inode);
    // check validity
    if (string(file_inode.owner) != username || (file_inode.type=='d' && file_inode.size>0))
        return;
    parent_entry_inode_block = parent_entries[parent_node_info.first_empty_entry_index].inode_block;
    parent_entries[parent_node_info.first_empty_entry_index].inode_block = 0;
    bool is_empty=true;
    for (unsigned int i = 0; i < FS_DIRENTRIES; i++){
        if (parent_entries[i].inode_block != 0){
            is_empty=false;
            break;
        }
    }
    unsigned int curr_block = parent_inode.blocks[parent_node_info.first_empty_block];
    if (is_empty)
    {
        for (unsigned int i = parent_node_info.first_empty_block; i < parent_inode.size - 1; i++)
        {
            parent_inode.blocks[i] = parent_inode.blocks[i+1];
        }
        parent_inode.size--;
        disk_writeblock(t_inode.inode_block, &parent_inode); // write back modified inode
        boost::lock_guard<boost::mutex> lock(free_block_lock);
        free_blocks.push(curr_block);
    }
    else{
        disk_writeblock(curr_block, &parent_entries); // write back modified entries
    }
    }
    }
    
    {
    boost::lock_guard<boost::mutex> lock(free_block_lock);
    // delete all blocks in the file (for dir, file_inode.size will be 0)
    for (unsigned int i = 0; i < file_inode.size; i++){
        free_blocks.push(file_inode.blocks[i]);
    }
    free_blocks.push(parent_entry_inode_block);
    }
    // send response
    string message = "FS_DELETE " + username + " " + pathname + '\0';
    send_response(message, connectionfd);
}

void FileServer::handle_readblock(istringstream &req, const string &username, const string &pathname, int connectionfd)
{
    string block_str;
    if (req >> block_str)
    {
        // check validity of block_str
        if(!isblock(block_str)) return;
        uint32_t block_id = stoi(block_str);
        if (block_id >= FS_MAXFILEBLOCKS) return;
        // check username and pathname valid or not
        vector<string> path_names;
        if (!path_parser(path_names, pathname))    return;
        target_inode t_inode = traverse_dir(username, path_names); // last inode
        if (!t_inode.valid){
            disk_locks[t_inode.parent_inode_block]->unlock_shared();
            return;
        }
        // read file_inode
        fs_inode file_inode;
        char data[FS_BLOCKSIZE];

        { // read lock (t_inode)
        boost::shared_lock<boost::shared_mutex> read_lock(*disk_locks[t_inode.inode_block]);
        if (t_inode.inode_block)
        {
            disk_locks[t_inode.parent_inode_block]->unlock_shared();
        }
        disk_readblock(t_inode.inode_block, &file_inode);
        // check validity
        if (string(file_inode.owner) != username || file_inode.type != 'f' || block_id >= file_inode.size)
            return;

        disk_readblock(file_inode.blocks[block_id], data);
        }
        // send response
        string message = "FS_READBLOCK " + username + " " + pathname + " " + block_str + '\0';
        // append read content
        message.append(data, FS_BLOCKSIZE);
        send_response(message, connectionfd);
    }
}

void FileServer::handle_writeblock(istringstream &req, const string &username, const string &pathname, int connectionfd)
{
    string block_str, data;
    if (req >> block_str)// double check
    {
        // check validity of block_str
        if (!isblock(block_str))
            return;
        uint32_t block_id = stoi(block_str);
        if (block_id >= FS_MAXFILEBLOCKS)
            return;
        // recv the data
        string data;
        array<char, FS_BLOCKSIZE> buf;
        // Call recv() enough times to consume all the data the client sends.
        ssize_t recvd = 0;
        ssize_t rval;
        do
        {
            rval = recv(connectionfd, buf.data(), buf.size()-recvd, 0);
            data.append(buf.data(), rval);
            recvd += rval;
            if (recvd >= FS_BLOCKSIZE || rval == 0) // TODO: check whether to return
                break;
            if (rval < 0)
                return;
        } while (rval);
        // if (data.length() < FS_BLOCKSIZE)
        //     return;
        // else{
        //     data = data.substr(0, FS_BLOCKSIZE);
        // }

        // check username and pathname valid or not
        vector<string> path_names;
        if (!path_parser(path_names, pathname))
            return;
        target_inode t_inode = traverse_dir(username, path_names); // last inode
        if (!t_inode.valid){
            disk_locks[t_inode.parent_inode_block]->unlock_shared();
            return;
        }
        
        // read file_inode
        fs_inode file_inode;
        { // write lock (t_inode)
        boost::unique_lock<boost::shared_mutex> write_lock(*disk_locks[t_inode.inode_block]);
        if (t_inode.inode_block)
        {
            disk_locks[t_inode.parent_inode_block]->unlock_shared();
        }
            
        disk_readblock(t_inode.inode_block, &file_inode);
        // check validity
        if (string(file_inode.owner) != username || file_inode.type != 'f' || block_id > file_inode.size) return;
        
        bool inode_change=false;
        if (block_id == file_inode.size)
        { // write to a new block
            // check if there is enough space in disk and dir
            unsigned int new_data_block;
            {
            boost::lock_guard<boost::mutex> lock(free_block_lock);
            if (free_blocks.empty() || file_inode.size == FS_MAXFILEBLOCKS)
                return; // space in disk
            // get the free block for the new file
            new_data_block = free_blocks.front();
            free_blocks.pop();
            }
            file_inode.blocks[file_inode.size] = new_data_block;
            file_inode.size++;
            inode_change = true;
        }
        // data write
        disk_writeblock(file_inode.blocks[block_id], data.c_str());
        if(inode_change){
            disk_writeblock(t_inode.inode_block, &file_inode);
        }
        }
        // send response
        string message = "FS_WRITEBLOCK " + username + " " + pathname + " " + block_str + '\0';
        send_response(message, connectionfd);
    }
}

int FileServer::check_request_type(const string& request_type, const string& req_header)
{
    // check the request type
    if (request_type == "FS_READBLOCK")
    {
        if (find_space(req_header) != 3)
            return 0;
        return 1;
    }
    if (request_type == "FS_WRITEBLOCK")
    {
        if (find_space(req_header) != 3)
            return 0;
        return 2;
    }
    if (request_type == "FS_CREATE")
    {
        if (find_space(req_header) != 3)
            return 0;
        return 3;
    }
    if (request_type == "FS_DELETE")
    {
        if (find_space(req_header) != 2)
            return 0;
        return 4;
    }
    return 0;
}

void FileServer::handle_requests(int connectionfd)
{
    // (1) Receive message from client.
    string req_header;
    array<char, 1> buf;
    
    // Call recv() enough times to consume all the data the client sends.
    ssize_t recvd = 0;
    ssize_t rval;
    do 
    {
        rval = recv(connectionfd, buf.data(), buf.size(), 0);
        if (buf[0] == 0) break;
        req_header.append(buf.data(), rval);
        recvd += rval;
        if (recvd > max_header_len || rval <= 0)
        {
            close(connectionfd);
            return;
        }
    } while (rval && buf[0] != 0); 

    // (2) Print out the message
    istringstream req(req_header);
    string request_type, username, pathname;
    if (req >> request_type >> username >> pathname)
    {
        // check length of username and pathname
        if (!(username.length() > FS_MAXUSERNAME || pathname.length() > FS_MAXPATHNAME))
        {
            switch (check_request_type(request_type, req_header))
            {
                case 1:
                    handle_readblock(req, username, pathname, connectionfd); 
                    break;
                case 2:
                    handle_writeblock(req, username, pathname, connectionfd);
                    break;
                case 3:
                    handle_create(req, username, pathname, connectionfd);
                    break;
                case 4:
                    handle_delete(username, pathname, connectionfd);
                    break;
                    
                default:
                    break;
            }
        }
    }
    // Close connection
    close(connectionfd);
}


int main(int argc, char* argv[])
{
    int port = 0;
    if (argc == 2)
        port = atoi(argv[1]);
    
    FileServer fs(port);
    fs.init_server();
    while (1)
    {
        int connectionfd = accept(fs.get_sockfd(), NULL, NULL);
        if (connectionfd == -1)
            continue;
        boost::thread t(&FileServer::handle_requests, &fs, connectionfd);
        t.detach();
    }
    return 0;
}