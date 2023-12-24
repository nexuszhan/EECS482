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

void loop1(void *a)
{
    auto id = static_cast<char *>(a);
    mutex1.lock();
    cout << "loop called with id " << id << endl;
    g++;
    cout << id << "\t" << g << endl;
    g++;
    cv1.wait(mutex1);
    thread::yield();
    cout << id << "\t" << g << endl;
    mutex1.unlock();
}

void loop2(void *a)
{
    auto id = static_cast<char *>(a);
    mutex1.lock();
    cout << "loop called with id " << id << endl;
    cv1.wait(mutex1);
    g++;
    cout << id << "\t" << g << endl;
    thread::yield();
    g++;
    cout << id << "\t" << g << endl;
    thread::yield();
    g++;
    cout << id << "\t" << g << endl;
    mutex1.unlock();
}

void loop3(void *a)
{
    auto id = static_cast<char *>(a);
    mutex1.lock();
    cout << "loop called with id " << id << endl;
    g++;
    cout << id << "\t" << g << endl;
    cv1.broadcast();
    g++;
    cout << id << "\t" << g << endl;
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
    cout << "hh2" << endl;
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}