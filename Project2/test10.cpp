#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;
// double yield
int g = 0;
int print_num = 0;

mutex mutex1;
mutex mutex2;
cv cv1;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex2.lock();
    cout << "loop called with id " << id << endl;

    for (i = 0; i < 5; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << endl;
        thread::yield();

        cv1.wait(mutex2);

        print_num = 0;

        mutex2.unlock();

        mutex2.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex2.unlock();
}

void print1(void *nothing)
{
    for (int i = 0; i < 3; i++)
    {
        mutex1.lock();
        cout << "EECS\n";
        print_num++;
        thread::yield();
        mutex1.unlock();
    }
}

void print2(void *nothing)
{
    for (int i = 0; i < 3; i++)
    {
        mutex2.lock();
        cout << "482\n";
        print_num++;
        // thread::yield();
        mutex2.unlock();
    }
    cv1.signal();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread p1(loop, static_cast<void *>(const_cast<char *>("child thread")));
    thread p2(print2, nullptr);

    // thread t1(loop, static_cast<void *>(const_cast<char *>("child thread")));
    // loop(static_cast<void *>(const_cast<char *>("parent thread")));
    print1(nullptr);
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}