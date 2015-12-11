#ifndef __VTHREADING_H__
#define __VTHREADING_H__
#pragma once

class VCoreApi VSyncCriticalSection
{
#if VPLATFORM_WIN32
	CRITICAL_SECTION m_cs;
public:
	inline VSyncCriticalSection()	{ InitializeCriticalSection(&m_cs);}
	inline ~VSyncCriticalSection()	{ DeleteCriticalSection(&m_cs);}
	inline void Lock()				{ EnterCriticalSection(&m_cs);}
	inline void Unlock()			{ LeaveCriticalSection(&m_cs);}
	inline BOOL TryLock()			{ return TryEnterCriticalSection(&m_cs);}
#else 
	pthread_mutex_t m_mutex;	// POSIX mutex
public:
	inline VSyncCriticalSection()	{ pthread_mutex_init(&m_mutex, NULL);}
	inline ~VSyncCriticalSection()	{ pthread_mutex_destroy(&m_mutex);}
	inline void Lock()				{ pthread_mutex_lock(&m_mutex);}
	inline void Unlock()			{ pthread_mutex_unlock(&m_mutex);}
	inline BOOL TryLock()			{ return pthread_mutex_trylock(&m_mutex) == 0;}
#endif 
};

class VCoreApi VSyncEvent
{
public:
	VSyncEvent(void);
	~VSyncEvent(void);

	BOOL Create(BOOL ManualReset = FALSE,const char* InName = NULL);
	void Destory();

	void Trigger(void);
	void Reset(void);
	// WaitTime : 等待时间(ms)
	BOOL Wait(UINT WaitTime = (UINT)-1);

	inline BOOL IsValid() const 
	{
#if VPLATFORM_WIN32
		return m_hevent != NULL;
#else 
		return m_event != NULL;
#endif 
	}
private:
#if VPLATFORM_WIN32
	HANDLE m_hevent;
#else 
	struct PThreadEvent* m_event;
#endif 
};


template<typename Locker>
class VAutoLocker
{
	Locker& m_Locker;
public:
	inline VAutoLocker(Locker& locker):m_Locker(locker)
	{
		m_Locker.Lock();
	}
	inline ~VAutoLocker()
	{
		m_Locker.Unlock();
	}
};

typedef VAutoLocker<VSyncCriticalSection> VAutoLockerCS;



inline void VSpinLock(volatile int* ptr, int PrevValue, int NewValue) 
{
	while(VInterlockedCompareExchange(ptr, NewValue, PrevValue) != PrevValue)
	{
	}
}

class VSyncSpinLock
{
public:
	inline VSyncSpinLock(volatile int* lock):m_lock(lock)
	{
		VSpinLock(m_lock, 0, 1);
	}
	inline ~VSyncSpinLock() 
	{
		VSpinLock(m_lock,1,0);
	}
private:
	volatile int* m_lock;
};

class VAtomInt
{
public:
	inline VAtomInt():m_Value(0)
	{
	}
	inline VAtomInt( const VAtomInt& Other ):m_Value(Other.GetValue())
	{
	}
	inline VAtomInt(int Value):m_Value(Value)
	{
	}
	inline int Inc()
	{
		return VInterlockedIncrement(&m_Value);
	}

	inline int Add(int Amt)
	{
		return VInterlockedAdd(&m_Value, Amt);
	}

	inline int Dec()
	{
		return VInterlockedDecrement(&m_Value);
	}

	inline int Sub(int Amt)
	{
		return VInterlockedAdd(&m_Value, -Amt);
	}

	inline int Set(int Value)
	{
		return VInterlockedExchange(&m_Value, Value);
	}

	inline int GetValue() const
	{
		return m_Value;
	}
private:
	volatile int m_Value;

	void operator=(const VAtomInt& Other){}
};


enum EVThreadPriority
{
	VTP_Low = 0,
	VTP_Normal,
	VTP_High,
};

// thread. 
class VCoreApi VThread
{
public:
	VThread();
	virtual ~VThread();
	BOOL Run(UINT StackSize = 0);

	BOOL Terminate(BOOL Wait = FALSE);
	void WaitForCompletion(void);
	UINT GetThreadID(void);

	// TODO : 下面的接口在平台移植的时候会很麻烦, 尽量避免....
	void SetThreadPriority(EVThreadPriority NewPriority);
	void SetProcessorAffinity(DWORD ProcessorNum);
	void Suspend();
	void Resume();

	void SetName(const char* ThreadName);
protected:
	virtual UINT Process() = 0;
private:
#if VPLATFORM_WIN32
	static DWORD WINAPI _ThreadProc(LPVOID data);
	HANDLE m_hThread;
	HANDLE m_hEvent;
	UINT m_ThreadId;
#elif VPLATFORM_WINRT
	static int _ThreadProc(void *data);
	thrd_t m_hThread;
	cnd_t m_hCond;
	mtx_t m_hMutex;
#else	// POSIX thread for android and ios , 
	static void *_ThreadProc(void *data);
	pthread_t m_hThread;
	volatile bool mRuning;
#endif
	EVThreadPriority m_Priority;
};

class VCoreApi VThreadPool
{

};


// 异步工作
class VCoreApi VAsyncWork
{
public:
	void WaitComplete(){}
	virtual void DoWork() = 0;
};

#endif 
