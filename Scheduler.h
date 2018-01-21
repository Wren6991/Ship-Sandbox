/***************************************************************************************
* Original Author:		Luke Wren (wren6991@gmail.com)
* Created:				2013-04-30
* Modified By:			Gabriele Giuseppini
* Copyright:			Luke Wren (http://github.com/Wren6991),
*						Gabriele Giuseppini  (https://github.com/GabrieleGiuseppini)
***************************************************************************************/
#pragma once

#include <mutex>
#include <queue>
#include <thread>

class Scheduler
{
public:

    struct ITask
    {
		virtual ~ITask() {}

        virtual void Process() = 0;        
    };

public:
	
	Scheduler();
	~Scheduler();

	Scheduler(Scheduler const &) = delete;
	Scheduler & operator=(Scheduler const &) = delete;
	
	void Schedule(ITask *t);

	void WaitForAllTasks();

	size_t GetNumberOfThreads() const { return mNThreads; }

private:

    class Thread
    {
	public:
		Thread(
			Scheduler * parent,
			int name);

		static void Enter(void * thisThread);

	private:
        Scheduler * const mParent;
		int const mName;
		std::thread mThread;
		ITask * mCurrentTask;
    };

    class Semaphore
    {
	public:
		Semaphore()
			: mCount(0)
		{}
		void Signal();
		void Wait();

	private:
        std::mutex mMutex;
        std::condition_variable mCondition;
        unsigned long mCount;
    };

    size_t const mNThreads;
    std::vector <Thread*> mThreadPool;
    Semaphore mAvailable;
    Semaphore mCompleted;
    std::queue<ITask*> mTasks;
    std::mutex mCritical;
};
