// TODO
/***************************************************************
* Name:      titanicApp.h
* Purpose:   Defines Application Class
* Author:    Luke Wren (wren6991@gmail.com)
* Created:   2013-04-30
* Copyright: Luke Wren (http://github.com/Wren6991)
* License:
**************************************************************/

#include "scheduler.h"

#include <iostream>

// scheduler

scheduler::scheduler()
	: mNThreads(std::thread::hardware_concurrency())
{
    for (int i = 0; i < mNThreads; i++)
    {
        mThreadPool.push_back(new thread(this));
		mThreadPool[i]->name = i;
    }
}

scheduler::~scheduler()
{
    /*for (unsigned int i = 0; i < threadPool.size(); i++)
        threadPool[i];*/
}

void scheduler::schedule(task *t)
{
	std::lock_guard<std::mutex> lock(mCritical);

    mTasks.push(t);
    mAvailable.signal();
}

void scheduler::wait()
{
	int tasksleft;
	{
		std::lock_guard<std::mutex> lock(mCritical);
		tasksleft = mTasks.size();
	}

    for (int i = 0; i < tasksleft; i++)	
        mCompleted.wait();
}

int scheduler::getNThreads()
{
    return mNThreads;
}

// scheduler::semaphore

void scheduler::semaphore::signal()
{
    std::lock_guard<std::mutex> lock(mMutex);

    ++mCount;
    mCondition.notify_one();
}

void scheduler::semaphore::wait()
{
    std::unique_lock<std::mutex> lock(mMutex);

    while (!mCount)
        mCondition.wait(lock);
    --mCount;
}

// scheduler::thread

scheduler::thread::thread(scheduler * parent)
	: mParent(parent)
{
	// Start thread
	mThread = std::thread(scheduler::thread::enter, this);
	mThread.detach();
}

void scheduler::thread::enter(void *arg)
{
    scheduler::thread *_this = static_cast<scheduler::thread*>(arg);
	while (true)
	{
		// Wait for a task to become available
		_this->mParent->mAvailable.wait();

		// Deque the task
		{
			std::lock_guard<std::mutex> lock(_this->mParent->mCritical);
			
			_this->mCurrentTask = _this->mParent->mTasks.front();
			_this->mParent->mTasks.pop();
		}

        // Run the task
        _this->mCurrentTask->process();

		// Notify completion
        delete _this->mCurrentTask;
        _this->mParent->mCompleted.signal();
   }
}
