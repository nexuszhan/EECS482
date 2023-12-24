#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;

mutex mutex1;
// cv cv1;
thread *t3;
thread *t4;

void loop1(void *a)
{
    auto id = static_cast<char *>(a);
    int i;
    mutex1.lock();
    cout << "loop1 called with id " << id << endl;
    mutex1.unlock();
    t4->join();
    mutex1.lock();
    for (i = 0; i < 5; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex1.unlock();
        thread::yield();
        mutex1.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void loop2(void *a)
{
    auto id = static_cast<char *>(a);
    int i;
    mutex1.lock();
    cout << "loop2 called with id " << id << endl;
    thread::yield();
    for (i = 0; i < 5; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex1.unlock();
        thread::yield();
        mutex1.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    // mutex1.unlock();

    thread t1(loop1, static_cast<void *>(const_cast<char *>("child thread1")));
    t3 = &t1;
    thread t2(loop2, static_cast<void *>(const_cast<char *>("child thread2")));
    t4 = &t2;
    mutex1.unlock();
    cout << "hh2" << endl;
    t3->join();
    cout << "hh3" << endl;
    // loop(static_cast<void *>(const_cast<char *>("parent thread")));
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}