#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <queue>
#include "tinythread.h"

class scheduler
{
public:
    struct task
    {
        virtual void process() = 0;
        virtual ~task() {}
    };
private:
    class thread: public tthread::thread
    {
        scheduler *parent;
        task *currentTask;
    public:
        int name;
        thread(scheduler *_parent);
        static void enter(void *_this);
    };
    class semaphore
    {
        tthread::mutex m;
        tthread::condition_variable condition;
        unsigned long count_;
    public:
        semaphore(): count_(0) {}
        void signal();
        void wait();
    };
    int nthreads;
    std::vector <thread*> threadPool;
    semaphore available;
    semaphore completed;
    std::queue<task*> tasks;
    tthread::mutex critical;
public:
    scheduler();
    ~scheduler();
    void schedule(task *t);
    void wait();
    int getNThreads();
};


#endif // _SCHEDULER_H_
