/* Define the cpu::cpu() in class cpu, member functions in class thread 
and some helper functions */

#include <stdexcept>
#include "cpu.h"
#include "thread.h"
#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;

std::shared_ptr<TCB> running_thread;
queue<std::shared_ptr<TCB>> ready_queue;
ucontext_t *host_context_ptr; // the context of cpu::cpu
std::shared_ptr<TCB> previous_thread;

TCB::TCB()
{
    ucontext_ptr = new ucontext_t();
    stack_ptr = new char[STACK_SIZE];
    ucontext_ptr->uc_stack.ss_sp = (void *)stack_ptr; 
    ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr->uc_stack.ss_flags = 0;
    ucontext_ptr->uc_link = nullptr;
}

TCB::~TCB()
{
    delete[] stack_ptr;
    delete ucontext_ptr;
}

void starter(thread_startfunc_t func, void *arg)
{
    previous_thread.reset();

    cpu::interrupt_enable();
    func(arg);
    cpu::interrupt_disable();

    auto &temp_queue = running_thread->join_queue; 
    while (!temp_queue.empty())
    {
        ready_queue.push(temp_queue.front());
        temp_queue.pop();
    }
    
    if (!ready_queue.empty())
    {
        previous_thread = running_thread;
        running_thread.reset();

        running_thread = ready_queue.front();
        ready_queue.pop();

        setcontext(running_thread->ucontext_ptr);
    }
    else // go back to cpu if there is no ready threads
    {
        setcontext(host_context_ptr);
    }
}

// void create_thread(thread_startfunc_t func, void *arg)
// {
//     try
//     {
//         // TCB *tcb_ptr = new TCB();
//         std::shared_ptr<TCB> tcb_ptr = std::make_shared<TCB>();
//         // alive_thread_set[total_thread] = tcb_ptr;
//         // total_thread++;
//         makecontext(tcb_ptr->ucontext_ptr, reinterpret_cast<void (*)()>(starter), 2, func, arg);

//         ready_queue.push(tcb_ptr);
//         tcb_ptr.reset();
//     }
//     catch (std::bad_alloc &e)
//     {
//         cpu::interrupt_enable();
//         throw std::bad_alloc();
//     }
// }

cpu::cpu(thread_startfunc_t func, void *arg)
{
    interrupt_vector_table[TIMER] = &(thread::yield);

    // create_thread(func, arg);
    // create the TCB of the parent function
    try
    {
        std::shared_ptr<TCB> tcb_ptr = std::make_shared<TCB>();
        makecontext(tcb_ptr->ucontext_ptr, reinterpret_cast<void (*)()>(starter), 2, func, arg);

        ready_queue.push(tcb_ptr);
        tcb_ptr.reset();
    }
    catch (std::bad_alloc &e)
    {
        cpu::interrupt_enable();
        throw std::bad_alloc();
    }

    // initialize the context of cpu::cpu
    host_context_ptr = new ucontext_t();
    getcontext(host_context_ptr);

    while (!ready_queue.empty())
    {
        running_thread = ready_queue.front();
        ready_queue.pop();

        swapcontext(host_context_ptr, running_thread->ucontext_ptr);

        previous_thread.reset();
    }
    delete host_context_ptr;
    running_thread.reset();
    previous_thread.reset();

    cpu::interrupt_enable_suspend();
}

thread::thread(thread_startfunc_t func, void *arg)
{
    cpu::interrupt_disable();

    // create_thread(func, arg);
    try
    {
        std::shared_ptr<TCB> tcb_ptr = std::make_shared<TCB>();
        makecontext(tcb_ptr->ucontext_ptr, reinterpret_cast<void (*)()>(starter), 2, func, arg);

        ready_queue.push(tcb_ptr);
        tcb = tcb_ptr;
        tcb_ptr.reset();
    }
    catch (std::bad_alloc &e)
    {
        cpu::interrupt_enable();
        throw std::bad_alloc();
    }

    cpu::interrupt_enable();
}

thread::~thread()
{
}

/* swap to the front of ready queue of ready is non-empty; 
   swap to the cpu::cpu otherwise */
void swap_helper()
{
    if (!ready_queue.empty())
    {
        // TCB *tmp = running_thread;
        ucontext_t* oucp = running_thread->ucontext_ptr;
        running_thread = ready_queue.front();
        ready_queue.pop();

        swapcontext(oucp, running_thread->ucontext_ptr);
    }
    else // go back to cpu if there is no ready threads
    {
        swapcontext(running_thread->ucontext_ptr, host_context_ptr);
    }
    previous_thread.reset();
}

void thread::yield()
{
    // assert_interrupts_enabled();
    cpu::interrupt_disable();
    
    if (!ready_queue.empty())
    {
        ready_queue.push(running_thread);
        swap_helper();
    }

    cpu::interrupt_enable();
}

void thread::join()
{
    // assert_interrupts_enabled();
    cpu::interrupt_disable();

    if (!tcb.expired())
    {
        tcb.lock()->join_queue.push(running_thread);
        swap_helper();
    }

    cpu::interrupt_enable();
}
