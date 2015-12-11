/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	VPlatform_Android.h
//	Created: 2014-1-18   13:31
//
**********************************************************************************/

#undef VPLATFORM_MOBILE
#define VPLATFORM_MOBILE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <wctype.h>
#include <pthread.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <utime.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
//#include <sys/atomics.h>
#include <math.h>

#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef UINT64
#undef BOOL

typedef unsigned char		BYTE;
typedef unsigned short	WORD;
typedef unsigned short	USHORT;
typedef unsigned int	UINT;
typedef unsigned int		DWORD;
typedef uint64_t	UINT64;

typedef	signed char		SBYTE;
typedef signed short		SHORT;
typedef signed int 		INT;
typedef long				LONG;
typedef int64_t		INT64;
typedef char				CHAR;
typedef wchar_t				WCHAR;

typedef signed int		BOOL;
typedef float				FLOAT;
typedef double				DOUBLE;


#define TRUE 1
#define FALSE 0

VCoreApi extern DOUBLE vg_SecondsPerCycle;
VCoreApi extern UINT64 vg_AppStartCycle;
inline UINT64 VGetCycles() 
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (UINT64) ((((UINT64)tv.tv_sec) * 1000000) + ((UINT64)tv.tv_usec));
}

inline DOUBLE VGetTime()
{
	UINT64 Cycles = VGetCycles() - vg_AppStartCycle;
	return (DOUBLE)(Cycles * vg_SecondsPerCycle);
}

inline UINT VGetCurrentThreadId()
{
	return pthread_self();
}


// atomic
inline INT VInterlockedIncrement(volatile INT* Value)
{
	// note: __sync_ atomic funtion return the previouse value. 
	return __sync_fetch_and_add(Value, 1) + 1;
}

inline INT VInterlockedDecrement(volatile INT* Value)
{
	return __sync_fetch_and_sub(Value, 1) - 1;
}

inline INT VInterlockedAdd(volatile INT* Value, INT Amount)
{
	return __sync_fetch_and_add(Value, Amount);
}

inline INT VInterlockedExchange(volatile INT* Value, INT Exchange)
{
	return __sync_lock_test_and_set(Value, Exchange);
}

inline INT VInterlockedCompareExchange(volatile INT* Dest, INT Exchange, INT Comperand)
{
	return __sync_val_compare_and_swap(Dest, Comperand, Exchange);
}
inline void* VInterlockedCompareExchangePointer(void** Dest, void* Exchange, void* Comperand)
{
	return __sync_val_compare_and_swap(Dest, Comperand, Exchange);
}


inline void VSleep(UINT MSec)
{
	if (MSec > 0)
	{
		usleep(MSec * 1000);
	}
	else
	{
		sched_yield();
	}
}

inline UINT GetCurrentThreadId()
{
	return gettid();
}

#define CDECL 