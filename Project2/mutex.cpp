/* Defines the member functions in class mutex */

#include <stdexcept>
#include "cpu.h"
#include "mutex.h"
#include <iostream>
#include <assert.h>

using std::cout;
using std::endl;

mutex::mutex()
{
    status = false;
}

void mutex::lock()
{
    // assert_interrupts_enabled();
    cpu::interrupt_disable();
    if (status) // mutex is not free
    {
        mutex_queue.push(running_thread);
        swap_helper();
    }
    else
    {
        status = true;
        occupy_thread = running_thread;
    }
    cpu::interrupt_enable();
}

bool mutex::check_ownership()
{
    return !occupy_thread.expired() && status && occupy_thread.lock().get() == running_thread.get();
}

void mutex::unlock()
{
    // assert_interrupts_enabled();
    cpu::interrupt_disable();
    
    if (check_ownership())
    {
        status = false;
        if (!mutex_queue.empty())
        {
            occupy_thread = mutex_queue.front();
            ready_queue.push(mutex_queue.front());
            mutex_queue.pop();
            status = true;
        }
    }
    else
    {
        cpu::interrupt_enable();
        throw std::runtime_error("An error occurred: try to unlock an unobtained lock.");
    }
    
    cpu::interrupt_enable();
}