#include <iostream>
#include <queue>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"
#include <string>

using std::cout;
using std::endl;
using std::queue;
using std::string;
using std::to_string;

int g = 0;
int hg = 0;

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
        while (hg < 3)
        {
            cv1.wait(mutex1);
        }
        cout << id << ":\t" << i << "\t" << g << " hg:" << hg << endl;
        hg++;

        // mutex1.unlock();
        // thread::yield();
        // mutex1.lock();
        cv1.signal();
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

    for (i = 0; i < 5; i++, g++)
    {
        cout << id << ":\t" << i << "\t" << g << " hg:" << hg << endl;
        hg++;
        mutex1.unlock();
        thread::yield();
        mutex1.lock();
        cv1.signal();
    }
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);
    string name0[10];
    name0[0] = "child thread0";
    name0[1] = "child thread1";
    name0[2] = "child thread2";
    name0[3] = "child thread3";
    name0[4] = "child thread4";

    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();
    for (int i = 0; i <= 4; i++)
    {
        // string t_name = "child thread" + to_string(i);
        // string t_name1 = t_name;
        // thread t1(loop, static_cast<void *>(const_cast<char *>(t_name1.c_str())));
        thread t1(loop, static_cast<void *>(const_cast<char *>(name0[i].c_str())));
    }
    thread t2(loop, static_cast<void *>(const_cast<char *>("child thread2")));
    loop1(static_cast<void *>(const_cast<char *>("parent thread")));
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}