/*
 * mutex.h -- interface to the mutex class
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#include "thread.h"

class mutex
{
public:
    mutex();
    ~mutex(){};

    void lock();
    void unlock();

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    mutex(const mutex &) = delete;
    mutex &operator=(const mutex &) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    mutex(mutex &&);
    mutex &operator=(mutex &&);

    bool status = false; // false when no one holds the lock
    std::weak_ptr<TCB> occupy_thread; // current owner of the mutex
    queue<std::shared_ptr<TCB>> mutex_queue; // threads waiting for this mutex

    bool check_ownership(); // check if the mutex is locked by the running thread
};

