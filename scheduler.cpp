#include "scheduler.h"

#include <iostream>
// scheduler

scheduler::scheduler()
{
    int nthreads = wxThread::GetCPUCount() * 2;
    for (int i = 0; i < nthreads; i++)
    {
        threadPool.push_back(new thread(this));
        threadPool[i]->name = i;
        threadPool[i]->Create();
        threadPool[i]->Run();
    }
}

scheduler::~scheduler()
{
    for (unsigned int i = 0; i < threadPool.size(); i++)
        threadPool[i]->Kill();
}

void scheduler::schedule(task *t)
{
    critical.Lock();
    tasks.push(t);
    available.Post();
    critical.Unlock();
}

void scheduler::wait()
{
    critical.Lock();
    while (completed.TryWait() != wxSEMA_BUSY);
    int tasksleft = tasks.size();
    critical.Unlock();
    for (int i = 0; i < tasksleft; i++)
        completed.Wait();
}

// scheduler::thread

scheduler::thread::thread(scheduler *_parent): wxThread(wxTHREAD_DETACHED)
{
    parent = _parent;
}

wxThread::ExitCode scheduler::thread::Entry()
{
    while (true)
    {
        //parent->critical.Lock(); std::cout << "Thread " << name << " ready.\n"; parent->critical.Unlock();
        parent->available.Wait();
        parent->critical.Lock();
        currentTask = parent->tasks.front();
        parent->tasks.pop();
        parent->critical.Unlock();
        //parent->critical.Lock(); std::cout << "Thread " << name << " starting task.\n"; parent->critical.Unlock();
        currentTask->process();
        delete currentTask;
        parent->completed.Post();
        //parent->critical.Lock(); std::cout << "Thread " << name << " finished.\n"; parent->critical.Unlock();
   }
    return 0;
}
