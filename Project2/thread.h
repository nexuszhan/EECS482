/*
 * thread.h -- interface to the thread library
 *
 * This file should be included by the thread library and by application
 * programs that use the thread library.
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#if !defined(__cplusplus) || __cplusplus < 201700L
#error Please configure your compiler to use C++17 or C++20
#endif

#include <memory>
#include <ucontext.h>
#include <queue>

using std::queue;

static constexpr unsigned int STACK_SIZE = 262144; // size of each thread's stack in bytes

using thread_startfunc_t = void (*)(void *);
class TCB;
class thread
{
public:
    thread(thread_startfunc_t func, void *arg); // create a new thread
    ~thread();

    void join(); // wait for this thread to finish

    static void yield(); // yield the CPU

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    thread(const thread &) = delete;
    thread &operator=(const thread &) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    thread(thread &&);
    thread &operator=(thread &&);

    std::weak_ptr<TCB> tcb; // used to track the TCB of the thread
};

class TCB
{
public:
    ucontext_t *ucontext_ptr;
    queue<std::shared_ptr<TCB>> join_queue;
    char *stack_ptr; // the original address of stack
    
    TCB();
    ~TCB();
};

void swap_helper();

extern std::shared_ptr<TCB> running_thread;
extern queue<std::shared_ptr<TCB>> ready_queue;
// extern ucontext_t *host_context_ptr;