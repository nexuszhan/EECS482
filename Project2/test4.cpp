#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;
// join train
mutex mutex1;
cv cv1;
thread *t4;
thread *t5;
thread *t6;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    cout << "loop called with id " << id << endl;
    t4->join();
    cout << "loop1 called with id " << id << endl;
}

void loop1(void *a)
{
    auto id = static_cast<char *>(a);
    cout << "loop called with id " << id << endl;
    t5->join();
    cout << "loop1 called with id " << id << endl;
}

void loop2(void *a)
{
    auto id = static_cast<char *>(a);

    cout << "loop called with id " << id << endl;
    t6->join();
    cout << "loop1 called with id " << id << endl;
}

void loop3(void *a)
{
    auto id = static_cast<char *>(a);
    thread::yield();
    cout << "loop called with id " << id << endl;
    thread::yield();
    cout << "loop called with id " << id << endl;
    thread::yield();
    cout << "loop called with id " << id << endl;
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1(loop1, static_cast<void *>(const_cast<char *>("child thread1")));
    thread t2(loop2, static_cast<void *>(const_cast<char *>("child thread2")));
    thread t3(loop3, static_cast<void *>(const_cast<char *>("child thread3")));
    t4 = &t1;
    t5 = &t2;
    t6 = &t3;
    cout << "loop called with id hh1" << endl;
    thread::yield();
    cout << "loop called with id hh" << endl;
    loop(static_cast<void *>(const_cast<char *>("parent thread")));
    cout << "parent called with arg " << arg << endl;
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}