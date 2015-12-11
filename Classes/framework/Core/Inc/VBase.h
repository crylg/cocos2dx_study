/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	VBase.h
//	Created: 2004-11-14   4:29
//
**********************************************************************************/
#ifndef __VBASE_H__
#define __VBASE_H__
#pragma once

#if !defined(VPLATFORM_WIN32) && !defined(VPLATFORM_IOS) && !defined(VPLATFORM_ANDROID) && !defined(VPLATFORM_WINRT)
#define VPLATFORM_WIN32 1
#define VPLATFORM_ANDROID 0 
#define VPLATFORM_IOS	0
#define VPLATFORM_WINRT 0
#endif 

//#define VPLATFORM_IOS 
//#define VPLATFORM_ANDROID

#ifndef VCoreApi
#ifdef _WIN32
	#define VCoreApi	__declspec(dllimport)
#else
	#define VCoreApi	__attribute__((visibility("default")))
#endif
#endif 


#ifndef VDEBUG 
#define VDEBUG 0
#endif 

#ifndef VRELEASE 
#define VRELEASE 0
#endif 

#ifndef VPROFILE
#define VPROFILE 0
#endif 

// version
// VDEBUG 
// VPROFILE  undebug version but with some beachmark and profiler code
// VRELEASE  release version with clear code!	 
#ifdef _DEBUG
#undef VDEBUG
#define VDEBUG 1
#endif 



#if _MSC_VER
#define VALIGNED_PREFIX(n)	__declspec(align(n))
#define VALIGNED_SUFFIX(n)  
#define VALIGNED(TYPE, N) __declspec(align(N)) TYPE
#define VALIGNED_DATA( type, name, n ) __declspec(align(n)) type name
#else	// gnu
#define VALIGNED_PREFIX(n)
#define VALIGNED_SUFFIX(n) __attribute__((aligned(n)))
#define VALIGNED(TYPE, N) TYPE __attribute__ ((aligned(N)))
#define VALIGNED_DATA( type, name, n ) type __attribute__ ((aligned(n))) name;
#endif




#include "VPlatform.h"


#define VSTRUCT_OFFSET(Struct, Field)	offsetof(Struct, Field)



#ifndef VCRT_SECURE
#define VCRT_SECURE 0
#endif 

//VARCH_X86
//VARCH_X64
//VARCH_ARM
#include <stdint.h>
#include <assert.h>
#define VASSERT				assert
#define VERROR(x,y)
#define VBIT(x)  (1<<x)


#ifdef DONT_USE_EXCEPTION
#include "Log.h"
#define NUM_EXCEPTIONS	32
extern volatile int exception_depth;
extern jmp_buf exception_buf[NUM_EXCEPTIONS];
extern const char *exception_data[NUM_EXCEPTIONS];
#ifdef _CELLOS_LV2
#include <cell/atomic.h>
#define TRY { cellAtomicIncr32((uint32_t*)&exception_depth); assert(exception_depth < NUM_EXCEPTIONS && "try(): stack overflow"); if(setjmp(exception_buf[exception_depth]) == 0) { {
#define CATCH(EXP) } cellAtomicDecr32((uint32_t*)&exception_depth); } else { EXP = exception_data[cellAtomicDecr32((uint32_t*)&exception_depth)]; assert(exception_depth >= 1 && "catch(): stack underflow"); {
#elif _ANDROID
#include <sys/atomics.h>
#define TRY { __atomic_inc(&exception_depth); assert(exception_depth < NUM_EXCEPTIONS && "try(): stack overflow"); if(setjmp(exception_buf[exception_depth]) == 0) { {
#define CATCH(EXP) } __atomic_dec(&exception_depth); } else { EXP = exception_data[__atomic_dec(&exception_depth)]; assert(exception_depth >= 1 && "catch(): stack underflow"); {
#else
#define TRY { exception_depth++; assert(exception_depth < NUM_EXCEPTIONS && "try(): stack overflow"); if(setjmp(exception_buf[exception_depth]) == 0) { {
#define CATCH(EXP) } exception_depth--; } else { EXP = exception_data[exception_depth--]; assert(exception_depth >= 1 && "catch(): stack underflow"); {
#endif
#define THROW(EXP) { if(exception_depth <= 1) Log::fatal("%s:%d: throw(): unhandled exception: %s",__FILE__,__LINE__,EXP); exception_data[exception_depth] = EXP; longjmp(exception_buf[exception_depth],1); }
#define TRY_END } } }
#else
#define TRY try
#define THROW(EXP) throw(EXP)
#define CATCH(EXP) catch(EXP)
#define TRY_END
#endif

//#define VWARN(...) 
#include "VLog.h"
#include <stdio.h>

#define VNAMESPACE_BEGIN(ns) namespace ns {
#define VNAMESPACE_END(ns)		}	
#define VNAMESPACE_USE(ns)  using namespace ns


//////////////////////////////////////////////////////////////////////////
// crt function wrap
//////////////////////////////////////////////////////////////////////////

inline char* VStrcpy(char* Dest, size_t DestSize, const char* Src)
{
#if VCRT_SECURE
	strcpy_s(Dest, DestSize, Src);
	return Dest;
#else 
	return strcpy(Dest, Src);
#endif 
}

inline char* VStrncpy(char* Dest, size_t DestSize, const char* Src, size_t Count)
{
#if VCRT_SECURE
	strncpy_s(Dest, DestSize, Src, Count);
	return Dest;
#else
	return strncpy(Dest, Src, Count);
#endif
}

inline char* VStrcat(char* Dest, size_t DestSize, const char* Src)
{
#if VCRT_SECURE
	strcat_s(Dest, DestSize, Src);
	return Dest;
#else 
	return strcat(Dest, Src);
#endif
}

inline int VSscanf(const char* Src, const char* Format, ...)
{
	va_list kArgs;
	va_start(kArgs, Format);
#if VCRT_SECURE
	int iRet = sscanf_s(Src, Format, kArgs);
	va_end(kArgs);
	return iRet;
#else
	int iRet = sscanf(Src, Format, kArgs);
	va_end(kArgs);
	return iRet;
#endif
}


inline char* LStrtok(char* pcString, const char* pcDelimit, char** ppcContext)
{
#if _MSC_VER >= 1400
	return strtok_s(pcString, pcDelimit, ppcContext);
#else // #if _MSC_VER >= 1400
	return strtok(pcString, pcDelimit);
#endif // #if _MSC_VER >= 1400
}


inline int LVsnprintf(char* pcDest, size_t stDestSize, size_t stCount, const char* pcFormat, va_list kArgs)
{
//	if (stDestSize == 0)
//	{
//		return 0;
//	}
//
//	VASSERT(pcDest);
//	VASSERT(stCount < stDestSize || stCount == (size_t)-1);
//	VASSERT(pcFormat);
//
//	// Ensure that input buffer is cleared out.
//	pcDest[0] = '\0';
//
//	BOOL bTruncate = (stCount == (size_t)-1);
//
//#if _MSC_VER >= 1400
//	int iRet = vsnprintf_s(pcDest, stDestSize, stCount, pcFormat, kArgs);
//#else   // _MSC_VER >= 1400
//	if (bTruncate)
//	{
//		stCount = stDestSize - 1;
//	}
//	int iRet = _vsnprintf(pcDest, stCount, pcFormat, kArgs);
//#endif  // _MSC_VER >= 1400
//
//	if (iRet == -1 && !bTruncate)
//	{
//		iRet = (int)stCount;
//	}
//
//#if _MSC_VER < 1400
//	// Ensure that the string ends in a null character.
//	if (iRet == -1)
//	{
//		pcDest[stDestSize - 1] = '\0';
//	}
//	else
//	{
//		pcDest[iRet] = '\0';
//	}
//#endif  // #if _MSC_VER < 1400
//
//	return iRet;
	return -1;
}

inline int LVsprintf(char* pcDest, size_t stDestSize, const char* pcFormat, va_list kArgs)
{
	return LVsnprintf(pcDest, stDestSize, (size_t)-1, pcFormat, kArgs);
}

inline int VSprintf(char* pcDest, size_t stDestSize, const char* pcFormat, ...)
{
	VASSERT(pcFormat);
	va_list kArgs;
	va_start(kArgs, pcFormat);
	int iRet = LVsprintf(pcDest, stDestSize, pcFormat, kArgs);
	va_end(kArgs);
	return iRet;
}


inline int LSnprintf(char* pcDest, size_t stDestSize, size_t stCount, const char* pcFormat, ...)
{
	VASSERT(pcFormat);

	va_list kArgs;
	va_start(kArgs, pcFormat);
	int iRet = LVsnprintf(pcDest, stDestSize, stCount, pcFormat, kArgs);
	va_end(kArgs);

	return iRet;
}

inline void VMemcpy(void* Dest, size_t DestSize, const void* Src, size_t Count)
{
#if VCRT_SECURE
	memcpy_s(Dest, DestSize, Src, Count);	// return -1 ??? check!!
#else 
	memcpy(Dest, Src, Count);
#endif
}

inline void VMemcpy(void* Dest, const void* Src, size_t Count)
{
	VMemcpy(Dest, Count, Src, Count);
}

inline void VMemmove(void* Dest, size_t DestSize, const void* Src, size_t Count)
{
#if VCRT_SECURE
	memmove_s(Dest, DestSize, Src, Count);	// check return value -1 ??
#else 
	memmove(Dest, Src, Count);
#endif 
}

inline void VMemmove(void* Dest, const void* Src, size_t Count)
{
#if VCRT_SECURE
	memmove_s(Dest, Count, Src, Count);	// check return value -1 ??
#else 
	memmove(Dest, Src, Count);
#endif 
}


inline WCHAR* LWStrcpy(WCHAR* pkDest, size_t stDestSize, const WCHAR* pkSrc)
{

#if _MSC_VER >= 1400
	wcscpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
	return pkDest;
#else // #if _MSC_VER >= 1400
	VASSERT(stDestSize != 0);
	size_t stSrcLen = wcslen(pkSrc);
	VASSERT(stDestSize > stSrcLen); // > because need null character
	size_t stWrite;

	if (stDestSize <= stSrcLen) // stDestSize < stSrcLen + 1
		stWrite = stDestSize;
	else 
		stWrite = stSrcLen + 1;

	WCHAR* pkResult = wcsncpy(pkDest, pkSrc, stWrite);
	pkResult[stDestSize - 1] = 0;
	return pkResult;
#endif // #if _MSC_VER >= 1400

}

inline WCHAR* LWStrncpy(WCHAR* pkDest, size_t stDestSize, const WCHAR* pkSrc, size_t stCount)
{

#if _MSC_VER >= 1400
	wcsncpy_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc, stCount);
	return pkDest;
#else // #if _MSC_VER >= 1400
	VASSERT(pkDest != 0 && stDestSize != 0);
	VASSERT(stCount < stDestSize);
	WCHAR* pkResult = wcsncpy(pkDest, pkSrc, stCount);
	pkResult[stCount] = 0;
	return pkResult;
#endif // #if _MSC_VER >= 1400
}

inline WCHAR* LWStrcat(WCHAR* pkDest, size_t stDestSize, const WCHAR* pkSrc)
{

#if _MSC_VER >= 1400
	wcscat_s((wchar_t *)pkDest, stDestSize, (const wchar_t *)pkSrc);
	return pkDest;
#else // #if _MSC_VER >= 1400
	size_t stSrcLen = wcslen(pkSrc);
	size_t stDestLen = wcslen(pkDest);
	VASSERT(stSrcLen + stDestLen <= stDestSize - 1);
	size_t stWrite = stDestSize - 1 - stDestLen;  

	WCHAR* pkResult = wcsncat(pkDest, pkSrc, stWrite);
	pkResult[stDestSize - 1] = '\0';
	return pkResult;

#endif // #if _MSC_VER >= 1400

}

inline WCHAR* LWStrtok(WCHAR* pkString, const WCHAR* pkDelimit, WCHAR** ppkContext)
{
#if _MSC_VER >= 1400
	return (WCHAR *)wcstok_s((wchar_t *)pkString,
		(const wchar_t *)pkDelimit, (wchar_t **)ppkContext);
#else // #if _MSC_VER >= 1400
	return wcstok(pkString, pkDelimit, ppkContext);
#endif // #if _MSC_VER >= 1400

}

inline int LWSprintf(WCHAR* pkDest, size_t stDestSize, const WCHAR* pkFormat, ...)
{

	if (stDestSize == 0)
		return 0;

	va_list kArgs;
	va_start(kArgs, pkFormat);

#if _MSC_VER >= 1400
	int iRet = vswprintf_s((wchar_t *)pkDest, stDestSize,
		(const wchar_t *)pkFormat, kArgs);
#else // #if _MSC_VER >= 1400
	int iRet = vswprintf(pkDest, stDestSize, pkFormat, kArgs);

	if (iRet >= 0 && ((unsigned int)iRet == stDestSize - 1) &&
		pkDest[stDestSize - 1] != '\0')
	{
		// This is a rare case where the written string fits but
		// is not null terminated. We will report this as an error.
		iRet = -1;
	}
#endif // #if _MSC_VER >= 1400

	va_end(kArgs);

	pkDest[stDestSize - 1] = '\0';

	return iRet;
}

inline int VStrnicmp(const char* s1, const char* s2, size_t n)
{
#if _MSC_VER >= 1400
	return _strnicmp(s1, s2, n);
#else // #if _MSC_VER >= 1400
#if VPLATFORM_WIN32
	return strnicmp(s1, s2, n);
#else 
	return strncasecmp(s1,s2,n);
#endif 
#endif // #if _MSC_VER >= 1400
}

inline int VStricmp(const char* s1, const char* s2)
{
#if _MSC_VER >= 1400
	return _stricmp(s1, s2);
#else // #if _MSC_VER >= 1400
#if VPLATFORM_WIN32
	return stricmp(s1, s2);
#else 
	return strcasecmp(s1, s2);
#endif 
#endif // #if _MSC_VER >= 1400
}

VCoreApi extern const BYTE vc_LowerMap[256];
VCoreApi extern const BYTE vc_UpperMap[256];

inline char VToLower(char c) { return (char)vc_LowerMap[c]; }
inline char VToUpper(char c) { return (char)vc_UpperMap[c]; }

inline const char* VStrStr(const char* Str, const char* Sub)
{
	return strstr(Str, Sub);
}
inline char* VStrStr(char* Str, const char* Sub)
{
	return strstr(Str, Sub);
}

VCoreApi const char* VStrIStr(const char* Str, const char* Sub);
inline char* VStrIStr(char* Str, const char* Sub)
{
	return (char*)VStrIStr((const char*)Str,Sub);
}


#include "VMemory.h"


#endif 

