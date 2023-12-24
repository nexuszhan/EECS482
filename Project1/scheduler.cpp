#include <iostream>
#include <utility>
#include <queue>
#include <vector>
#include <fstream>
#include <string>

#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"
#include "disk.h"

using std::cout;
using std::endl;
using std::queue;
using std::vector;
using std::pair;
using std::min;

mutex mutex1;
cv requester_cv;
cv server_cv;

int cur_track = 0;
unsigned int max_disk_queue = 0;
unsigned int requester_num = 0;
vector<char *> file_names;
vector<pair<unsigned int, unsigned int>> disk_queue;
int disk_queue_full = 0;
vector<int> requester_done;

void server_func(void *nothing)
{
    while (true)
    {
        mutex1.lock();

        while (!disk_queue_full)
        {
            server_cv.wait(mutex1);
        }
        
        int diff = 1000;
        unsigned int target_index = 0;
        for (unsigned int i=0; i<disk_queue.size(); i++)
        {
            int tmp = static_cast<int>(disk_queue[i].second);
            if (std::abs(cur_track-tmp) < diff)
            {
                target_index = i;
                diff = std::abs(cur_track-tmp);
            }
        }
        cur_track = disk_queue[target_index].second;
        unsigned int req_i = disk_queue[target_index].first;
        disk_queue.erase(disk_queue.begin()+target_index);
        print_service(req_i, cur_track);
        
        if (requester_done[req_i] == -1)
            requester_num--;

        if (disk_queue.size() == min(requester_num, max_disk_queue))
            disk_queue_full = 1;
        else
            disk_queue_full = 0; 
        
        requester_done[req_i] = 1;
        requester_cv.broadcast();
        mutex1.unlock();

        if (!requester_num) 
            break;
    }
}

void requester_func(void *req_i_ptr)
{
    auto req_i = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(req_i_ptr));
    std::ifstream inputFile(file_names[req_i]);
    queue<unsigned int> tracks;

    std::string line;
    while (std::getline(inputFile, line))
    {
        unsigned int track = static_cast<unsigned int>(std::stoul(line));
        tracks.push(track);
    }
    inputFile.close();

    while (!tracks.empty())
    {
        unsigned int track = tracks.front();

        mutex1.lock();

        while (disk_queue_full || !requester_done[req_i])
        {
            requester_cv.wait(mutex1);
        }
        requester_done[req_i] = 0;
        disk_queue.emplace_back(req_i, track);
        tracks.pop();
        print_request(req_i, track);

        if (disk_queue.size() == min(requester_num, max_disk_queue))
            disk_queue_full = 1;

        // requester_done[req_i] = -1 means the ith requester will be inactive after serviced
        if (tracks.empty()) 
            requester_done[req_i] = -1;

        server_cv.signal();
        mutex1.unlock();
    }
}

void schedule(void *nothing)
{
    requester_done.resize(requester_num, 1);
    
    vector<thread> requesters;
    requesters.reserve(requester_num);
    for (uintptr_t i=0; i<requester_num; i++)
        requesters.emplace_back(requester_func, reinterpret_cast<void *>(i));
    thread server(server_func, nullptr);

    for (auto & requester : requesters)
        requester.join(); 
    server.join();
}

int main(int argc, char* argv[])
{
    max_disk_queue = std::atoi(argv[1]);

    disk_queue.reserve(max_disk_queue);
    requester_num = static_cast<unsigned int>(argc-2);

    file_names.reserve(requester_num);

    for (int i=2; i<argc; i++)
        file_names.push_back(argv[i]);
    
    cpu::boot(schedule, nullptr, 0);
}