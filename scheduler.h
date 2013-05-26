#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <queue>
#include <wx/thread.h>

class scheduler
{
public:
    struct task
    {
        virtual void process() = 0;
    };
private:
    class thread: public wxThread
    {
        scheduler *parent;
        task *currentTask;
    public:
        int name;
        thread(scheduler *_parent);
        virtual void *Entry();
    };
    std::vector <thread*> threadPool;
    wxSemaphore available;
    wxSemaphore completed;
    std::queue<task*> tasks;
    wxMutex critical;
public:
    scheduler();
    ~scheduler();
    void schedule(task *t);
    void wait();
};


#endif // _SCHEDULER_H_
