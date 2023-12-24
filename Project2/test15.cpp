#include <ucontext.h>
#include <memory>
#include <utility>
#include <stdexcept>
#include "cpu.h"
#include "mutex.h"
#include "cv.h"
#include "thread.h"
#include <iostream>
#include <assert.h>
#include <vector>
#include <string>

const int NUM_THREADS = 3;
const int NUM_ITER = 15;

int counter = 0;
mutex counter_mutex;
cv counter_cv;

void child(void *a)
{
    std::cout<<"child called"<<std::endl;
    counter_mutex.unlock();
    std::cout << "child successfully unlock without lock" << std::endl;
}

void ready(void *a)
{
    auto id = static_cast<char *>(a);
    std::cout<<"ready called"<<std::endl;
    counter_mutex.lock();
    std::cout << id << "ready lock without unlock " << counter << std::endl;
}

void parent(void *a)
{
    std::vector<thread*> threads;
    std::cout<<"parent called"<<std::endl;
    string s[10];
    s[0] = "thread 0";
    thread rd(ready, static_cast<void *>(const_cast<char *>("ready")));
    rd.join();
    thread t1(child, static_cast<void *>(const_cast<char *>(s[0].c_str())));

    std::cout << "parent finish" << counter << std::endl;
}

int main()
{
    cpu::boot(1, parent, nullptr, true, true, 0);
}