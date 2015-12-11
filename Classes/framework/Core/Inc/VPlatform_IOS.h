/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	VPlatform_IOS.h
//	Created: 2014-1-18   13:32
//
**********************************************************************************/
#undef VPLATFORM_MOBILE
#define VPLATFORM_MOBILE 1

#include <sys/time.h>
#include <unistd.h>
// threading
#include <pthread.h>
#include <libkern/OSAtomic.h>

VCoreApi extern DOUBLE vg_SecondsPerCycle;
VCoreApi extern UINT64 vg_AppStartCycle;
inline UINT64 VGetCycles() 
{
	return (UINT64)mach_absolute_time();
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

#define CDECL 