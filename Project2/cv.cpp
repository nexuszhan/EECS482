/* Defines the member functions of class cv */

#include <stdexcept>
#include "cpu.h"
#include "mutex.h"
#include "cv.h"
#include <iostream>
#include <assert.h>

cv::cv()
{
}

void cv::wait(mutex &m)
{
    
    // assert_interrupts_enabled();
    cpu::interrupt_disable();

    // check if the running thread hold the lock
    if (!m.check_ownership())
    {
        cpu::interrupt_enable();
        throw std::runtime_error("An error occurred: try to wait on an unobtained lock.");
    }
    
    cv_queue.push(running_thread);

    m.status = false;
    if (!m.mutex_queue.empty())
    {
        ready_queue.push(m.mutex_queue.front());
        m.status = true;
        m.occupy_thread = m.mutex_queue.front();
        m.mutex_queue.pop();
    }

    swap_helper();

    // re-acquire lock
    if (!m.status)
    {
        m.occupy_thread = running_thread;
        m.status = true;
    }
    else
    {
        m.mutex_queue.push(running_thread);
        swap_helper();
    }

    cpu::interrupt_enable();
}

void cv::signal()
{
    // assert_interrupts_enabled();
    cpu::interrupt_disable();
    if (!cv_queue.empty())
    {
        ready_queue.push(cv_queue.front());
        cv_queue.pop();
    }
    cpu::interrupt_enable();
}

void cv::broadcast()
{
    // assert_interrupts_enabled();
    cpu::interrupt_disable();
    while (!cv_queue.empty())
    {
        ready_queue.push(cv_queue.front());
        cv_queue.pop();
    }
    cpu::interrupt_enable();
}
