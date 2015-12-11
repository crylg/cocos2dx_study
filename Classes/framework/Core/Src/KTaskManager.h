#pragma once

#if VPLATFORM_WIN32

#include "VTask.h"

class KTaskManager;
/**
 *  缓存的工作线程
 *
 * @param 
 */
class KPoolThread : public VThread, public VAllocator
{
	virtual UINT Process();
	KTaskManager* mManager;
	VSyncEvent mEvent;
	VTaskBase* volatile mTask;
	volatile int mDie;
public:
	KPoolThread(KTaskManager* TaskManager):mManager(TaskManager),mTask(NULL),mDie(0)
	{
		mEvent.Create();
	}

	void DoTask(VTaskBase* Task)
	{
		VASSERT(mTask == NULL);

		mTask = Task;
		VMemoryBarrier();
			// Tell the thread to wake up and do its job
		mEvent.Trigger();
	}

	void Stop()
	{
		VInterlockedIncrement(&mDie);
		mEvent.Trigger();
		WaitForCompletion();
	}
};


/**
 *  其实就是一个线程池
 *
 * @param 
 */
class KTaskManager
{
	VSyncCriticalSection mCriticalSection;
	VPODArray<KPoolThread*> mIdleThreads;		// 处于休眠的线程
	VPODArray<KPoolThread*> mTheads;			// 所有创建的线程.
	VPODArray<VTaskBase*>  mTaskQueue;
public:
	KTaskManager(void);
	~KTaskManager(void);

	BOOL Init(int NumWorkThreads);

	void Shutdown();

	void RunTask(VTaskBase* Task);

	VTaskBase* PeekTaskOrPool(KPoolThread* WorkingThead);

	inline void UpdateTaskWorkingStatus(VTaskBase* Task)
	{
		VASSERT(Task->mStatus.GetValue() == 1);
		VASSERT(Task->mEvent.IsValid());
		Task->mStatus.Dec();
		Task->mEvent.Trigger();
	}

	bool TimeToDie;

public:

	//virtual bool RetractQueuedWork(FQueuedWork* InQueuedWork) override
	//{
	//	if (TimeToDie)
	//	{
	//		return false; // no special consideration for this, refuse the retraction and let shutdown proceed
	//	}
	//	check(InQueuedWork != NULL);
	//	check(SynchQueue);
	//	FScopeLock sl(SynchQueue);
	//	return !!QueuedWork.RemoveSingle(InQueuedWork);
	//}
};

extern KTaskManager* vTaskManager;

#endif 
