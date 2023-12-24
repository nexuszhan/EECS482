#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;
// a cv 2 mutex wake up
mutex mutex1;
mutex mutex2;
cv cv1;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    cout << "loop called with id " << id << endl;

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

void loop1(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    cout << "loop called with id " << id << endl;
    cv1.wait(mutex1);
    cout << "loop called with id " << id << endl;

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

    mutex2.lock();
    cout << "loop called with id " << id << endl;
    cv1.wait(mutex2);
    cout << "loop called with id " << id << endl;

    for (i = 0; i < 5; i++, g++)
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
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1(loop1, static_cast<void *>(const_cast<char *>("child thread1")));
    thread t2(loop2, static_cast<void *>(const_cast<char *>("child thread2")));
    thread::yield();
    cv1.broadcast();
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}