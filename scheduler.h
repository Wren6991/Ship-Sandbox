////////////////////////////////////////////////////////////////////////////////////////////
// 
// Original code by Luke Wren (https://github.com/Wren6991/Ship-Sandbox)
//
// Modified by Gabriele Giuseppini (https://github.com/GabrieleGiuseppini/Ship-Sandbox)
//
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <mutex>
#include <queue>
#include <thread>

class scheduler
{
public:
    struct task
    {
        virtual void process() = 0;
        virtual ~task() {}
    };

private:
    class thread
    {
	private:
        scheduler * const mParent;
		std::thread mThread;

		task * mCurrentTask;

    public:
        int name;
        thread(scheduler * parent);
        static void enter(void *_this);
    };

    class semaphore
    {
	private:
        std::mutex mMutex;
        std::condition_variable mCondition;
        unsigned long mCount;

    public:
        semaphore()
			: mCount(0) 
		{}
        void signal();
        void wait();
    };

    int const mNThreads;
    std::vector <thread*> mThreadPool;
    semaphore mAvailable;
    semaphore mCompleted;
    std::queue<task*> mTasks;
    std::mutex mCritical;

public:
    scheduler();
    ~scheduler();
    void schedule(task *t);
    void wait();
    int getNThreads();
};
