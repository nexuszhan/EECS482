// test cv.wait() runtime_error

#include <iostream>
#include <stdexcept>
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

    for (i=0; i<5; i++, g++) {
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

    try
    {
        cv1.wait(mutex1);
    }
    catch(std::runtime_error& e)
    {
        cout << e.what() << endl;
    }
    exit(0);
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}