/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#include "Scheduler.h"

#include "Log.h"

#ifdef FLOATING_POINT_CHECKS
#ifdef _MSC_VER
#include <float.h>
#else
// Have no idea how to do this on other compilers...
#endif
#endif

//
// Scheduler
//

Scheduler::Scheduler()
	: mNThreads(std::thread::hardware_concurrency())
	, mThreadPool()
	, mAvailable()
	, mCompleted()
	, mTaskQueue()
	, mTaskQueueMutex()
    , mCurrentScheduledTasks(0u)
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

    {
        std::lock_guard<std::mutex> lock(mTaskQueueMutex);
        mTaskQueue.push(t);               
    }

    mAvailable.Signal();

    ++mCurrentScheduledTasks;
}

void Scheduler::WaitForAllTasks()
{
	for (; mCurrentScheduledTasks > 0; --mCurrentScheduledTasks)
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

	while (0 == mCount)
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
#ifdef FLOATING_POINT_CHECKS
#ifdef _MSC_VER
#include <float.h>
    // Enable all floating point exceptions except these
    unsigned int fp_control_state = _controlfp(_EM_INEXACT | _EM_UNDERFLOW, _MCW_EM);
#else
    // Have no idea how to do this on other compilers...
#endif
#endif

    Scheduler::Thread * thisThread = static_cast<Scheduler::Thread *>(arg);
	while (true)
	{
		// Wait for a task to become available
		thisThread->mParent->mAvailable.Wait();

		// Deque the task
		{
			std::lock_guard<std::mutex> lock(thisThread->mParent->mTaskQueueMutex);
			
			thisThread->mCurrentTask = thisThread->mParent->mTaskQueue.front();
			thisThread->mParent->mTaskQueue.pop();
		}

        // Run the task
		thisThread->mCurrentTask->Process();

		// Notify completion
        delete thisThread->mCurrentTask;
		thisThread->mParent->mCompleted.Signal();
   }
}
