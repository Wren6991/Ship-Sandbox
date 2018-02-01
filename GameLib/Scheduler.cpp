/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Scheduler.h"

#include "Log.h"


//
// Scheduler
//

Scheduler::Scheduler()
	: mNThreads(std::thread::hardware_concurrency())
	, mThreadPool()
	, mAvailable()
	, mCompleted()
	, mTasks()
	, mCritical()
{
	LogMessage("Number of threads: ", mNThreads);
}

Scheduler::~Scheduler()
{
	/*
	for (unsigned int i = 0; i < mThreadPool.size(); i++)
		mThreadPool[i];
		*/
}

void Scheduler::Schedule(ITask *t)
{
	if (mThreadPool.empty())
	{
		assert(mNThreads > 0);
		for (int i = 0; i < mNThreads; i++)
		{
			mThreadPool.push_back(new Thread(this, i + 1));
		}
	}

	std::lock_guard<std::mutex> lock(mCritical);

    mTasks.push(t);
    mAvailable.Signal();
}

void Scheduler::WaitForAllTasks()
{
	size_t tasksleft;
	{
		std::lock_guard<std::mutex> lock(mCritical);
		tasksleft = mTasks.size();
	}

	for (size_t i = 0; i < tasksleft; ++i)
	{
		mCompleted.Wait();
	}
}

//
// Semaphore
//

void Scheduler::Semaphore::Signal()
{
    std::lock_guard<std::mutex> lock(mMutex);

    ++mCount;
    mCondition.notify_one();
}

void Scheduler::Semaphore::Wait()
{
    std::unique_lock<std::mutex> lock(mMutex);

	while (!mCount)
	{
		mCondition.wait(lock);
	}

    --mCount;
}

//
// Thread
//

Scheduler::Thread::Thread(
	Scheduler * parent,
	int name)
	: mParent(parent)
	, mName(name)
{
	// Start thread
	mThread = std::thread(Scheduler::Thread::Enter, this);
	mThread.detach();
}

void Scheduler::Thread::Enter(void *arg)
{
    Scheduler::Thread * thisThread = static_cast<Scheduler::Thread *>(arg);
	while (true)
	{
		// Wait for a task to become available
		thisThread->mParent->mAvailable.Wait();

		// Deque the task
		{
			std::lock_guard<std::mutex> lock(thisThread->mParent->mCritical);
			
			thisThread->mCurrentTask = thisThread->mParent->mTasks.front();
			thisThread->mParent->mTasks.pop();
		}

        // Run the task
		thisThread->mCurrentTask->Process();

		// Notify completion
        delete thisThread->mCurrentTask;
		thisThread->mParent->mCompleted.Signal();
   }
}
