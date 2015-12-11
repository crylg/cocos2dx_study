#include "VStdAfx.h"
#include "VThreading.h"
#include <errno.h>
#if VPLATFORM_WIN32
VSyncEvent::VSyncEvent(void):m_hevent(NULL)
{

}

VSyncEvent::~VSyncEvent(void)
{
	if (m_hevent != NULL)
	{
		CloseHandle(m_hevent);
	}
}

///**
//* Waits for the event to be signaled before returning
//*/
//void VSyncEvent::Lock(void)
//{
//	WaitForSingleObject(m_hevent,INFINITE);
//}
//
///**
//* Triggers the event so any waiting threads are allowed access
//*/
//void VSyncEvent::Unlock(void)
//{
//	PulseEvent(m_hevent);
//}

BOOL VSyncEvent::Create(BOOL ManualReset,const char* InName)
{
	m_hevent = CreateEventA(NULL,ManualReset,0,InName);
	return m_hevent != NULL;
}

void VSyncEvent::Destory()
{
	if (m_hevent)
	{
		CloseHandle(m_hevent);
		m_hevent = NULL;
	}
}

void VSyncEvent::Trigger(void)
{
	VASSERT(m_hevent);
	SetEvent(m_hevent);
}

void VSyncEvent::Reset(void)
{
	VASSERT(m_hevent);
	ResetEvent(m_hevent);
}

// 
BOOL VSyncEvent::Wait(UINT WaitTime)
{
	VASSERT(m_hevent);
	return WaitForSingleObject(m_hevent,WaitTime) == WAIT_OBJECT_0;
}

//////////////////////////////////////////////////////////////////////////
// Thread
//////////////////////////////////////////////////////////////////////////

/**
 * Code setting the thread name for use in the debugger.
 *
 * http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
 */
#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;		// Must be 0x1000.
	LPCSTR szName;		// Pointer to name (in user addr space).
	DWORD dwThreadID;	// Thread ID (-1=caller thread).
	DWORD dwFlags;		// Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)


void SetThreadName( DWORD ThreadID, LPCSTR ThreadName )
{
	Sleep(10);
	THREADNAME_INFO ThreadNameInfo;
	ThreadNameInfo.dwType		= 0x1000;
	ThreadNameInfo.szName		= ThreadName;
	ThreadNameInfo.dwThreadID	= ThreadID;
	ThreadNameInfo.dwFlags		= 0;
	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof(ThreadNameInfo)/sizeof(ULONG_PTR), (ULONG_PTR*)&ThreadNameInfo );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
}


#elif VPLATFORM_ANDROID


/**
* This is the PThreads version of FEvent.
*/
class PThreadEvent
{
	// This is a little complicated, in an attempt to match Win32 Event semantics...
	typedef enum
	{
		TRIGGERED_NONE,
		TRIGGERED_ONE,
		TRIGGERED_ALL,
	} TriggerType;

	bool bInitialized;
	bool bIsManualReset;
	volatile TriggerType Triggered;
	volatile int WaitingThreads;
	pthread_mutex_t Mutex;
	pthread_cond_t Condition;

	inline void LockEventMutex()
	{
		const int rc = pthread_mutex_lock(&Mutex);
		VASSERT(rc == 0);
	}

	inline void UnlockEventMutex()
	{
		const int rc = pthread_mutex_unlock(&Mutex);
		VASSERT(rc == 0);
	}

	static inline void SubtractTimevals( const struct timeval *FromThis, struct timeval *SubThis, struct timeval *Difference )
	{
		if (FromThis->tv_usec < SubThis->tv_usec)
		{
			int nsec = (SubThis->tv_usec - FromThis->tv_usec) / 1000000 + 1;
			SubThis->tv_usec -= 1000000 * nsec;
			SubThis->tv_sec += nsec;
		}

		if (FromThis->tv_usec - SubThis->tv_usec > 1000000)
		{
			int nsec = (FromThis->tv_usec - SubThis->tv_usec) / 1000000;
			SubThis->tv_usec += 1000000 * nsec;
			SubThis->tv_sec -= nsec;
		}

		Difference->tv_sec = FromThis->tv_sec - SubThis->tv_sec;
		Difference->tv_usec = FromThis->tv_usec - SubThis->tv_usec;
	}

public:

	PThreadEvent()
	{
		bInitialized = false;
		bIsManualReset = false;
		Triggered = TRIGGERED_NONE;
		WaitingThreads = 0;
	}

~PThreadEvent()
	{
		// Safely destructing an Event is VERY delicate, so it can handle badly-designed
		//  calling code that might still be waiting on the event.
		if (bInitialized)
		{
			// try to flush out any waiting threads...
			LockEventMutex();
			bIsManualReset = true;
			UnlockEventMutex();
			Trigger();  // any waiting threads should start to wake up now.

			LockEventMutex();
			bInitialized = false;  // further incoming calls to this object will now crash in check().
			while (WaitingThreads)  // spin while waiting threads wake up.
			{
				UnlockEventMutex();  // cycle through waiting threads...
				LockEventMutex();
			}
			// No threads are currently waiting on Condition and we hold the Mutex. Kill it.
			pthread_cond_destroy(&Condition);
			// Unlock and kill the mutex, since nothing else can grab it now.
			UnlockEventMutex();
			pthread_mutex_destroy(&Mutex);
		}
	}

	bool Create( bool _bIsManualReset = false )
	{
		VASSERT(!bInitialized);
		bool RetVal = false;
		Triggered = TRIGGERED_NONE;
		bIsManualReset = _bIsManualReset;

		if (pthread_mutex_init(&Mutex, nullptr) == 0)
		{
			if (pthread_cond_init(&Condition, nullptr) == 0)
			{
				bInitialized = true;
				RetVal = true;
			}
			else
			{
				pthread_mutex_destroy(&Mutex);
			}
		}
		return RetVal;
	}

	 void Trigger()
	{
		VASSERT(bInitialized);

		LockEventMutex();

		if (bIsManualReset)
		{
			// Release all waiting threads at once.
			Triggered = TRIGGERED_ALL;
			int rc = pthread_cond_broadcast(&Condition);
			VASSERT(rc == 0);
		}
		else
		{
			// Release one or more waiting threads (first one to get the mutex
			//  will reset Triggered, rest will go back to waiting again).
			Triggered = TRIGGERED_ONE;
			int rc = pthread_cond_signal(&Condition);  // may release multiple threads anyhow!
			VASSERT(rc == 0);
		}

		UnlockEventMutex();
	}

	 void Reset() 
	{
		VASSERT(bInitialized);
		LockEventMutex();
		Triggered = TRIGGERED_NONE;
		UnlockEventMutex();
	}

	 bool Wait(UINT WaitTime = (UINT)-1, const bool bIgnoreThreadIdleStats = false)
	 {
		 VASSERT(bInitialized);

		 struct timeval StartTime;

		 // We need to know the start time if we're going to do a timed wait.
		 if ( (WaitTime > 0) && (WaitTime != ((UINT)-1)) )  // not polling and not infinite wait.
		 {
			 gettimeofday(&StartTime, NULL);
		 }

		 LockEventMutex();

		 bool bRetVal = false;

		 // loop in case we fall through the Condition signal but someone else claims the event.
		 do
		 {
			 // See what state the event is in...we may not have to wait at all...

			 // One thread should be released. We saw it first, so we'll take it.
			 if (Triggered == TRIGGERED_ONE)
			 {
				 Triggered = TRIGGERED_NONE;  // dibs!
				 bRetVal = true;
			 }

			 // manual reset that is still signaled. Every thread goes.
			 else if (Triggered == TRIGGERED_ALL)
			 {
				 bRetVal = true;
			 }

			 // No event signalled yet.
			 else if (WaitTime != 0)  // not just polling, wait on the condition variable.
			 {
				 WaitingThreads++;
				 if (WaitTime == ((UINT)-1)) // infinite wait?
				 {
					 int rc = pthread_cond_wait(&Condition, &Mutex);  // unlocks Mutex while blocking...
					 VASSERT(rc == 0);
				 }
				 else  // timed wait.
				 {
					 struct timespec TimeOut;
					 const UINT ms = (StartTime.tv_usec / 1000) + WaitTime;
					 TimeOut.tv_sec = StartTime.tv_sec + (ms / 1000);
					 TimeOut.tv_nsec = (ms % 1000) * 1000000;  // remainder of milliseconds converted to nanoseconds.
					 int rc = pthread_cond_timedwait(&Condition, &Mutex, &TimeOut);    // unlocks Mutex while blocking...
					 VASSERT((rc == 0) || (rc == ETIMEDOUT));

					 // Update WaitTime and StartTime in case we have to go again...
					 struct timeval Now, Difference;
					 gettimeofday(&Now, NULL);
					 SubtractTimevals(&Now, &StartTime, &Difference);
					 const int DifferenceMS = ((Difference.tv_sec * 1000) + (Difference.tv_usec / 1000));
					 WaitTime = ((DifferenceMS >= WaitTime) ? 0 : (WaitTime - DifferenceMS));
					 StartTime = Now;
				 }
				 WaitingThreads--;
				 VASSERT(WaitingThreads >= 0);
			 }

		 } while ((!bRetVal) && (WaitTime != 0));

		 UnlockEventMutex();
		 return bRetVal;
	 }
};


VSyncEvent::VSyncEvent(void)
{
	m_event = new PThreadEvent;
}

VSyncEvent::~VSyncEvent(void)
{
	delete m_event;
}
//
//void VSyncEvent::Lock(void)
//{
//	Wait((UINT)-1);  // infinite wait.
//}
//
//void VSyncEvent::Unlock(void)
//{
//	PulseEvent(m_hevent);
//}

BOOL VSyncEvent::Create(BOOL ManualReset,const char* InName)
{
	VASSERT(m_event);
	return m_event->Create(ManualReset ? true : false) ? true : false;
}

void VSyncEvent::Trigger(void)
{
	VASSERT(m_event);
	m_event->Trigger();
	//SetEvent(m_hevent);
}

void VSyncEvent::Reset(void)
{
	VASSERT(m_event);
	m_event->Reset();
	//ResetEvent(m_hevent);
}

// 
BOOL VSyncEvent::Wait(UINT WaitTime)
{
	VASSERT(m_event);
	return m_event->Wait(WaitTime) ? TRUE : FALSE;
}
#elif VPLATFORM_IOS

#else 

#endif 

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#if VPLATFORM_WIN32
DWORD WINAPI VThread::_ThreadProc(LPVOID data) 
{
	VThread* thread = (VThread*)data;
	thread->Process();
	return 0;
}
#elif VPLATFORM_WINRT
int VThread::_ThreadProc(void *data) {
	VThread* thread = (VThread*)data;
	thread->Process();
	return 0;
}
#else
void* VThread::_ThreadProc(void *data)
{
	VThread* thread = (VThread*)data;
#if defined(_LINUX) || defined(_MACOS) || defined(_IOS)
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
#endif
	thread->Process();

	thread->mRuning = false;
	pthread_exit(NULL);
	return NULL;
}
#endif

VThread::VThread():
#if VPLATFORM_WIN32
m_hThread(NULL),m_hEvent(NULL),m_ThreadId(0)
#else 
m_hThread(-1),mRuning(false)
#endif 
{

}

VThread::~VThread()
{

}

BOOL VThread::Run(UINT StackSize)
{
#if VPLATFORM_WIN32
	VASSERT(m_hThread == NULL);
	DWORD ThreadId;
	m_hThread = CreateThread(NULL,(SIZE_T)StackSize,&VThread::_ThreadProc,this,0,&ThreadId);
	m_ThreadId = (UINT)ThreadId;
return (m_hThread != NULL);
#elif VPLATFORM_WINRT
	return (thrd_create(&m_hThread,&VThread::_ThreadProc,this) == thrd_success);
#else
	mRuning = true;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,StackSize);
	pthread_create(&m_hThread,&attr,&VThread::_ThreadProc,this);
	return (pthread_detach(m_hThread) == 0);
#endif

}

void VThread::SetThreadPriority(EVThreadPriority NewPriority)
{

}
void VThread::SetProcessorAffinity(DWORD ProcessorNum)
{

}

void VThread::Suspend()
{
	
}

void VThread::Resume()
{
	
}

BOOL VThread::Terminate(BOOL bShouldWait)
{
	return TRUE;
}

void VThread::WaitForCompletion(void)
{
#if VPLATFORM_WIN32
	WaitForSingleObject(m_hThread,INFINITE);
#else 
	while (mRuning)
	{
		VSleep(1);
	}
#endif 
}

UINT VThread::GetThreadID(void)
{
	return 0;
}

void VThread::SetName(const char* ThreadName)
{
#if VPLATFORM_WIN32

	THREADNAME_INFO threadName;
	threadName.dwType = 0x1000;
	threadName.szName = ThreadName;
	threadName.dwThreadID = GetCurrentThreadId();
	threadName.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(threadName)/sizeof(DWORD), (ULONG_PTR*)&threadName );
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
#endif 
}