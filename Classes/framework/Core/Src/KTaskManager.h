#pragma once

#if VPLATFORM_WIN32

#include "VTask.h"

class KTaskManager;
/**
 *  ����Ĺ����߳�
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
 *  ��ʵ����һ���̳߳�
 *
 * @param 
 */
class KTaskManager
{
	VSyncCriticalSection mCriticalSection;
	VPODArray<KPoolThread*> mIdleThreads;		// �������ߵ��߳�
	VPODArray<KPoolThread*> mTheads;			// ���д������߳�.
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
