#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"
// test for join
using std::cout;
using std::endl;

int g = 0;

mutex mutex1;
// cv cv1;
void loop1(void *a);

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;
    // cout << "mm" << id << endl;
    // cout << "ms" << mutex1.occupy_thread->thread_id << endl;
    mutex1.lock();
    cout << "loop called with id " << id << endl;
    thread t1(loop1, static_cast<void *>(const_cast<char *>("child thread1")));
    for (i = 0; i < 2; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex1.unlock();
        t1.join();
        cout << "11" << endl;
        thread::yield();
        cout << "22" << endl;
        mutex1.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void loop1(void *a)
{
    auto id = static_cast<char *>(a);
    int i;
    // cout << "mm" << id << endl;
    // cout << "ms" << mutex1.occupy_thread->thread_id << endl;
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

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1(loop, static_cast<void *>(const_cast<char *>("child thread0")));
    t1.join();
    cout << "hh2" << endl;
    loop(static_cast<void *>(const_cast<char *>("parent thread")));
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), true, false, 0);
}