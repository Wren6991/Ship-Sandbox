#include "scheduler.h"

#include <iostream>
// scheduler

scheduler::scheduler()
{
    nthreads = tthread::thread::hardware_concurrency();
    for (int i = 0; i < nthreads; i++)
    {
        threadPool.push_back(new thread(this));
        threadPool[i]->name = i;
    }
}

scheduler::~scheduler()
{
    /*for (unsigned int i = 0; i < threadPool.size(); i++)
        threadPool[i];*/
}

void scheduler::schedule(task *t)
{
    critical.lock();
    tasks.push(t);
    available.signal();
    critical.unlock();
}

void scheduler::wait()
{
    critical.lock();
    int tasksleft = tasks.size();
    critical.unlock();
    for (int i = 0; i < tasksleft; i++)
        completed.wait();
}

int scheduler::getNThreads()
{
    return nthreads;
}

// scheduler::semaphore

void scheduler::semaphore::signal()
{
    tthread::lock_guard<tthread::mutex> lock(m);
    ++count_;
    condition.notify_one();
}

void scheduler::semaphore::wait()
{
    tthread::lock_guard<tthread::mutex> lock(m);
    while (!count_)
        condition.wait(m);
    --count_;
}

// scheduler::thread

scheduler::thread::thread(scheduler *_parent): tthread::thread(scheduler::thread::enter, this)
{
    parent = _parent;
    detach();
}

void scheduler::thread::enter(void *arg)
{
    scheduler::thread *_this = (scheduler::thread*) arg;
    while (true)
    {
        //parent->critical.lock(); std::cout << "Thread " << name << " ready.\n"; parent->critical.unlock();
        _this->parent->available.wait();
        _this->parent->critical.lock();
        _this->currentTask = _this->parent->tasks.front();
        _this->parent->tasks.pop();
        _this->parent->critical.unlock();
        //parent->critical.lock(); std::cout << "Thread " << name << " starting task.\n"; parent->critical.unlock();
        _this->currentTask->process();
        delete _this->currentTask;
        _this->parent->completed.signal();
        //parent->critical.lock(); std::cout << "Thread " << name << " finished.\n"; parent->critical.unlock();
   }
}
