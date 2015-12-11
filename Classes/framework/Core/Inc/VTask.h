#pragma once
#ifndef __VTASK_H__
#define __VTASK_H__
#include "VThreading.h"

/**
 *	多线程并行任务管理  
 *
 */



/**
 *	任务. 任务必须与Work 进行关联, 任务包含执行一个工作的信息.   
 *
 */
class VCoreApi VTaskBase
{
	enum
	{
		IDLE = 0,
		WORKING = 1,
		DONE = 2,
	};
public:

	/**
	 *  由任务调度线程调用, 执行任务. 
	 *
	 * @param 
	 */
	virtual void DoWork() = 0;

	// 主线程在关闭任务管理器时, 会让未开始执行的任务退出.  
	virtual void OnCancel() {}

	/**
	 *  等待任务执行完成
	 *
	 * @param 
	 */
	void WaitForCompleted();

	/**
	 *  开始执行异步工作. 
	 *
	 */
	void StartAsync();


	void StartSync();


	inline BOOL IsIdle() const { return mStatus.GetValue() == 0;}

private:
	VAtomInt mStatus;		// 0 : Idle 1: Progressing 2: Done
	VSyncEvent mEvent;
	friend class KTaskManager;
};


/*
		class UpdateJob
		{
		public:
			UpdateJob()
		};
*/



template<typename JobType>
class VTask : public VTaskBase, public VAllocator
{
	JobType mJob;
	virtual void DoWork() 
	{
		mJob.DoWork();
	}
public:

	inline JobType& GetWork() { return mJob; }

	VTask( )
	{
		
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T>
	VTask( T Arg )
		: mJob(Arg)
	{
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2>
	VTask( T1 Arg1, T2 Arg2 )
		: mJob(Arg1,Arg2)
	{
	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3 )
		: mJob(Arg1,Arg2,Arg3)
	{

	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3, typename T4>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4 )
		: mJob(Arg1,Arg2,Arg3,Arg4)
	{

	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3, typename T4, typename T5>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5 )
		: mJob(Arg1,Arg2,Arg3,Arg4,Arg5)
	{

	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3, typename T4, typename T5, typename T6>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6 )
		: mJob(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)
	{

	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7 )
		: mJob(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)
	{

	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8 )
		: mJob(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)
	{

	}
	/** Passthrough constructor. Generally speaking references will not pass through; use pointers */
	template<typename T1,typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	VTask( T1 Arg1, T2 Arg2, T3 Arg3, T4 Arg4, T5 Arg5, T6 Arg6, T7 Arg7, T8 Arg8, T9 Arg9 )
		: mJob(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)
	{
		
	}
};

class VCoreApi VTaskManager
{
public:
	static int NumTaskThreads();

	static bool AddTask();

	//static void RunAsync(VTask* Task);

};



#endif 