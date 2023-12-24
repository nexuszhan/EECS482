#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

int print_done = 0;
int g = 0;
int loop_done = 0;

mutex mutex1;
cv cv1;
cv print_cv;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i;

    mutex1.lock();
    cout << "loop called with id " << id << endl;

    for (i=0; i<5; i++, g++) 
    {
        while (!print_done)
        {
            cv1.wait(mutex1);
        }
        print_done = 0;
        cout << id << ":\t" << i << "\t" << g << endl;
        mutex1.unlock();
        //thread::yield();
        mutex1.lock();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    loop_done += 1;
    mutex1.unlock();

    cout << id << " loop done.\n";
}

void print(void* nothing)
{
    while (loop_done < 2)
    {
        mutex1.lock();
        cout << "EECS482\n";
        print_done = 1;
        mutex1.unlock();
        cv1.broadcast();
        thread::yield();
    }
    cout << "Print done.\n";
}

void caller2()
{
    thread t3 (print, nullptr);
}

void caller1()
{
    thread t2 (print, nullptr);

    caller2();
    
    thread t1 (loop, static_cast<void *>(const_cast<char *>("child thread")));
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);
    
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();
    
    caller1();
    loop(static_cast<void *>(const_cast<char *>("parent thread")));

    cout << "Parent done.\n";
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}