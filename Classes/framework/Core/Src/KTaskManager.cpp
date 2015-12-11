#include "VStdAfx.h"

#if VPLATFORM_WIN32

#include "KTaskManager.h"
 KTaskManager* vTaskManager = NULL;

UINT KPoolThread::Process() 
{
	SetName("AsyncTask");

	while (!mDie)
	{
		// Wait for some work to do
		mEvent.Wait();

		VTaskBase* LocalQueuedWork = mTask;
		mTask = NULL;
		VMemoryBarrier();
		VASSERT(LocalQueuedWork || mDie); // well you woke me up, where is the job or termination request?
		while (LocalQueuedWork)
		{
			LocalQueuedWork->DoWork();
			mManager->UpdateTaskWorkingStatus(LocalQueuedWork);

			// Let the object cleanup before we remove our ref to it
			LocalQueuedWork = mManager->PeekTaskOrPool(this);
		} 
	}
	return 0;
}

KTaskManager::KTaskManager(void)
{
}


KTaskManager::~KTaskManager(void)
{
}


BOOL KTaskManager::Init(int NumWorkThreads)
{

	VAutoLockerCS Lock(mCriticalSection);

	// 创建线程池 
	mTheads.Increment(NumWorkThreads, NULL);
	for (int i = 0; i< NumWorkThreads; ++i)
	{
		KPoolThread* PoolThread = VNew KPoolThread(this);
		if (!PoolThread->Run())
		{
			VDelete PoolThread;
			Shutdown();
			return FALSE;
		}
		mTheads[i] = PoolThread;

		// 刚刚创建的线程, 是一个空闲的线程. 
		mIdleThreads.PushBack(PoolThread);
	}
	
	mTheads.Compact();
	return TRUE;
}

void KTaskManager::Shutdown()
{
	{
		VAutoLockerCS Lock(mCriticalSection);
		TimeToDie = 1;
		VMemoryBarrier();

		// 首先清理正在等待执行的工作
		for (int Index = 0; Index < (int)mTaskQueue.Size(); Index++)
		{
			mTaskQueue[Index]->OnCancel();
		}
		mTaskQueue.Clear();

	}

	// wait for all threads to finish up
	while (1)
	{
		{
			VAutoLockerCS Lock(mCriticalSection);
			if (mTheads.Size() == mIdleThreads.Size())
			{
				break;
			}
		}
		VSleep(0);
	}

	// Delete all threads
	{
		VAutoLockerCS Lock(mCriticalSection);
		// Now tell each thread to die and delete those
		for (int Index = 0; Index < (int)mTheads.Size(); Index++)
		{
			KPoolThread* Thread = mTheads[Index];
			Thread->Stop();
			//Thread->WaitForCompletion();
			VDelete Thread;
		}
		mTheads.Clear();
		mIdleThreads.Clear();
	}
}

void KTaskManager::RunTask(VTaskBase* Task)
{
	// 如果我们已经销毁
	if (TimeToDie)
	{
		Task->OnCancel();
		return;
	}

	// 找到一个空闲的线程. 
	KPoolThread* Thread = NULL;
	VAutoLockerCS Lock(mCriticalSection);
	if (!mIdleThreads.IsEmpty())
	{
		Thread = mIdleThreads.GetLast();
		mIdleThreads.PopBack();
	}

	// Was there a thread ready?
	if (Thread != NULL)
	{
		// We have a thread, so tell it to do the work
		Thread->DoTask(Task);
	}
	else
	{
		// There were no threads available, queue the work to be done
		// as soon as one does become available
		//QueuedWork.Add(InQueuedWork);
		mTaskQueue.PushBack(Task);
	}
}

VTaskBase* KTaskManager::PeekTaskOrPool(KPoolThread* WorkingThead)
{
	VTaskBase* Task = NULL;
	// Check to see if there is any work to be done
	VAutoLockerCS Lock(mCriticalSection);
	if (TimeToDie)
	{
		VASSERT(mTaskQueue.IsEmpty());
		// we better not have anything if we are dying
	}
	if (!mTaskQueue.IsEmpty())
	{
		Task = mTaskQueue.GetFirst();
		mTaskQueue.EraseAt(0,1);
	}
	if (!Task)
	{
		// There was no work to be done, so add the thread to the pool
		mIdleThreads.PushBack(WorkingThead);
	}
	return Task;
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void VTaskBase::StartAsync()
{
	VASSERT(IsIdle());
	VMemoryBarrier();

	// 记录状态, 我们正在执行. 
	mStatus.Inc();

	if (!mEvent.IsValid())
	{
		mEvent.Create(TRUE);
		VASSERT(mEvent.IsValid());
	}
	
	mEvent.Reset();

	vTaskManager->RunTask(this);

}



void VTaskBase::WaitForCompleted()
{
	VMemoryBarrier();
	if (mEvent.IsValid())
	{
		mEvent.Wait();
	}
	VASSERT(IsIdle());
}

#endif 