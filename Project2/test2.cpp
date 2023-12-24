#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;

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

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    // mutex1.lock();
    // cv1.signal();
    // cv1.wait(mutex1);
    // cout << "parent called with arg " << arg << endl;
    // mutex2.lock();
    // cv1.wait(mutex2);
    // mutex1.unlock();

    cout << "hhh0" << endl;
    thread t1(loop, static_cast<void *>(const_cast<char *>("child thread")));
    cout << "hhh1" << endl;
    t1.join();
    cout << "hhh" << endl;

    loop(static_cast<void *>(const_cast<char *>("parent thread")));

    mutex2.lock();
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}