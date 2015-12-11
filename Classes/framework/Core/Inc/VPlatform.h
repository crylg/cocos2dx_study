/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	LPlatform.h
//	Created: 2004-7-12   20:47
//
**********************************************************************************/
#ifndef __VPLATFORM_H__
#define __VPLATFORM_H__
#pragma once

#define VPLATFORM_MOBILE 0


#if VPLATFORM_WIN32
#include "VPlatform_Win32.h"
#elif VPLATFORM_ANDROID
#include "VPlatform_Android.h"
#elif VPLATFORM_IOS
#include "VPlatform_IOS.h"
#else 
#error "Platform Not Specified."
#endif 

struct VSystemTime
{
	WORD Year;
	WORD Month;
	WORD Day;
	WORD DayOfWeek;
	WORD Hour;
	WORD Min;
	WORD Sec;
	WORD MSec;
};
extern VCoreApi void VGetSystemTime(VSystemTime& SystemTime);


struct VPlatformInfo
{
	UINT NumCore;
};


// internal function for initialize platform 
extern BOOL VPlatformInit();




#endif 

