#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int g = 0;

mutex mutex1;
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
    g++;
    mutex1.lock();
    cout << "loop called with id " << id << g << endl;
    thread::yield();
    g++;
    cout << "loop called with id " << id << g << endl;
    mutex1.unlock();
}

void loop2(void *a)
{
    auto id = static_cast<char *>(a);
    g++;
    mutex1.lock();
    cout << "loop called with id " << id << g << endl;
    cv1.wait(mutex1);
    g++;
    cout << "loop called with id " << id << g << endl;
    mutex1.unlock();
}

void loop3(void *a)
{
    auto id = static_cast<char *>(a);
    g++;
    mutex1.lock();
    cout << "loop called with id " << id << g << endl;
    cv1.broadcast();
    g++;
    cout << "loop called with id " << id << g << endl;
    thread::yield();
    g++;
    cout << "loop called with id " << id << g << endl;
    mutex1.unlock();
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

    thread::yield();
    thread::yield();
    thread::yield();

    loop(static_cast<void *>(const_cast<char *>("parent thread")));
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}