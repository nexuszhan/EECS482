#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

int main(int argc, char *argv[])
{
    char *server;
    int server_port;

    const char *writedata = "We hold these truths to be self-evident, that all men are created equal, that they are endowed by their Creator with certain unalienable Rights, that among these are Life, Liberty and the pursuit of Happiness. -- That to secure these rights, Governments are instituted among Men, deriving their just powers from the consent of the governed, -- That whenever any Form of Government becomes destructive of these ends, it is the Right of the People to alter or to abolish it, and to institute new Government, laying its foundation on such principles and organizing its powers in such form, as to them shall seem most likely to effect their Safety and Happiness.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3)
    {
        std::cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("user1", "/dir1", 'f');

    status = fs_create("user1", "/dir1/file1111", 'f');

    status = fs_create("user1", "/dir1/file1file1", 'd');

    status = fs_delete("user1", "/dir0/file0");

    status = fs_create("user1", "/dir1/file1111", 'f');

    status = fs_create("user1", "/dir1/file1file1", 'd');

    status = fs_delete("user1", "/dir0/file1");

    status = fs_create("user1", "/dir1/file1111", 'f');

    status = fs_create("user1", "/dir1/file1file1", 'd');

    status = fs_readblock("user1", "/dir/file", 0, readdata);

    status = fs_readblock("user1", "/dir", 0, readdata);

    status = fs_readblock("user1", "/dir/dir/dir/file", 0, readdata);

    status = fs_writeblock("user1", "/dir/dir/dir/file", 0, writedata);

    status = fs_writeblock("user1", "/dir/file", 1, writedata);

    status = fs_writeblock("user1", "/dir", 1, writedata);

    status = fs_writeblock("user2", "/dir1/file", 1, writedata);

    status = fs_readblock("user1", "/dir/file", 1, readdata);

    status = fs_delete("user1", "/dir");

    status = fs_delete("user2", "/dir1");

    status = fs_delete("user1", "/dir/file");

    status = fs_delete("user2", "/dir1/file");

    status = fs_delete("user1", "/dir/dir");

    status = fs_delete("user1", "/dir/dir/dir");

    status = fs_delete("user1", "/dir/dir/dir/file");

    status = fs_delete("user2", "/dir1");
}