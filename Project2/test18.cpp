#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;
// 1 cv for 2 mutex
mutex mutex1;
mutex mutex2;
cv cv1;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    mutex2.lock();
    cout << "loop called with id " << id << endl;
    cv1.broadcast();
    for (i = 0; i < 5; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex2.unlock();
        g++;
        thread::yield();
        mutex2.lock();
        mutex1.unlock();
        g++;
        thread::yield();
        mutex1.lock();
    }

    cout << id << ":\t" << i << "\t" << g << endl;
    mutex2.lock();
    mutex1.unlock();
}

void loop1(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    cout << "loop called with id " << id << endl;
    while (g == 0)
    {
        cv1.wait(mutex1);
    }
    for (i = 0; i < 3; i++, g++)
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

    mutex2.lock();
    cout << "loop called with id " << id << endl;
    while (g == 0)
    {
        cv1.wait(mutex2);
    }
    for (i = 0; i < 3; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex2.unlock();
        thread::yield();
        mutex2.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex2.unlock();
}

void parent(void *a)
{
    // auto arg = reinterpret_cast<intptr_t>(a);

    // mutex1.lock();
    // cv1.signal();
    // cv1.wait(mutex1);
    // cout << "parent called with arg " << arg << endl;
    // mutex2.lock();
    // cv1.wait(mutex2);
    // mutex1.unlock();

    thread t1(loop1, static_cast<void *>(const_cast<char *>("child thread1")));
    thread t2(loop2, static_cast<void *>(const_cast<char *>("child thread2")));

    loop(static_cast<void *>(const_cast<char *>("parent thread")));
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}