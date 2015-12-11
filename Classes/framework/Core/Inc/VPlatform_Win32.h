/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	VPlatform_Win32.h
//	Created: 2014-1-18   13:32
//
**********************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>
#include <tchar.h>
#include <wchar.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#if _MSC_VER >= 1400
#define VCRT_SECURE 1
#endif 

// 一些弱智的警告禁用
#pragma warning(disable : 4100)		//未引用的形参
#pragma warning(disable : 4514)		//未引用的内联函数已经被移开
#pragma warning(disable : 4201)		//使用了非标准扩展 : 无名称的结构/联合		
#pragma warning(disable : 4710)		//标记为 inline 的函数未内联 										*/
#pragma warning(disable : 4714)		//标记为 inline 的函数未内联
#pragma warning(disable : 4127)		// 条件表达式是常数
#pragma warning(disable : 4512)		// 未能生成赋值运算符
#pragma warning(disable : 4245)		// 枚举转换到LDWORD,
//#pragma warning(disable : 4018)		// 有/无符号不匹配
#pragma warning(disable : 4251)		/* class "A" 需要有 dll 接口由 class "B" 的客户端使用 */
#pragma warning(disable : 4273)		// DLL 链接不一致
#pragma warning(disable : 4275)		// 非DLL接口用作DLL接口
#pragma warning(disable : 4355)		// "this" : 用于基成员初始值设定项列表
#pragma warning(disable : 4324)		// 由于 __declspec(align())，结构被填充




#undef BYTE
#undef WORD
#undef DWORD
#undef INT
#undef FLOAT
#undef UINT64
#undef BOOL

typedef unsigned __int8		BYTE;
typedef unsigned __int16	WORD;
typedef unsigned __int16	USHORT;
typedef unsigned __int32	UINT;
typedef unsigned long		DWORD;
typedef unsigned __int64	UINT64;	

typedef	signed __int8		SBYTE;
typedef signed __int16		SHORT;
typedef signed __int32 		INT;
typedef long				LONG;	
typedef signed __int64		INT64;	
typedef char				CHAR;
typedef wchar_t				WCHAR;

typedef signed __int32		BOOL;
typedef float				FLOAT;
typedef double				DOUBLE;	



#if _WIN64
#else 
#define VARCH_X86 1
// TODO : winrt is arm.
#define V_SSE 1


VCoreApi extern DOUBLE vg_SecondsPerCycle;
VCoreApi extern UINT64 vg_AppStartCycle;
// get system clock cycles.
inline UINT64 VGetCycles() 
{
#if VARCH_X86
	LARGE_INTEGER tval;
	QueryPerformanceCounter(&tval);
	return (UINT64)tval.QuadPart;
#else
	return (UINT64)__rdtsc();
#endif
}

// get application time in second
inline DOUBLE VGetTime()
{
	UINT64 Cycles = VGetCycles() - vg_AppStartCycle;
	return (DOUBLE)(Cycles * vg_SecondsPerCycle);
}

// atomic
inline INT VInterlockedIncrement(volatile INT* Value)
{
	return (INT)InterlockedIncrement((LPLONG)Value);
}

inline INT VInterlockedDecrement(volatile INT* Value)
{
	return (INT)InterlockedDecrement((LPLONG)Value);
}

inline INT VInterlockedAdd(volatile INT* Value,INT Amount)
{
	return (INT)InterlockedExchangeAdd((LPLONG)Value,(LONG)Amount);
}

inline INT VInterlockedExchange(volatile INT* Value,INT Exchange)
{
	return (INT)InterlockedExchange((LPLONG)Value,(LONG)Exchange);
}

inline INT VInterlockedCompareExchange(volatile INT* Dest,INT Exchange,INT Comperand)
{
	return (INT)InterlockedCompareExchange((volatile LONG*)Dest,(LONG)Exchange,(LONG)Comperand);
}
inline void* VInterlockedCompareExchangePointer(void** Dest,void* Exchange,void* Comperand)
{
	return InterlockedCompareExchangePointer(Dest,Exchange,Comperand);
}

inline HANDLE VGetCurrentThread(void)	{	return GetCurrentThread();}
inline UINT VGetCurrentThreadId(void)	{	return (UINT)GetCurrentThreadId();}

inline void VSleep(UINT MSec)
{
	Sleep(MSec);
}


inline void VMemoryBarrier() {}


#ifdef CDECL
#undef CDECL
#endif 

#define CDECL __cdecl



extern "C" VCoreApi HINSTANCE hInstance; 


#endif 