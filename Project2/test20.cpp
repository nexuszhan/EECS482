#include <iostream>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;
// thread waking sequence
int g = 0;

mutex mutex1;
cv cv1;

void loop(void *a)
{
    auto id = static_cast<char *>(a);
    int i = 0;

    mutex1.lock();
    g++;
    cout << "loop called with id " << id << endl;
    cv1.signal();
    g++;
    cout << "loop called with id " << id << endl;
    cv1.wait(mutex1);
    g++;
    cout << id << ":\t" << i << "\t" << g << endl;
    cv1.signal();
    g++;
    cout << id << ":\t" << i << "\t" << g << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    auto arg = reinterpret_cast<intptr_t>(a);
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1(loop, static_cast<void *>(const_cast<char *>("child thread1")));
    thread t2(loop, static_cast<void *>(const_cast<char *>("child thread2")));
    mutex1.lock();
    cv1.wait(mutex1);
    mutex1.unlock();
    g++;
    cout << "p:\t" << g << endl;
    cv1.signal();

    loop(static_cast<void *>(const_cast<char *>("parent thread")));
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}