#include "VStdAfx.h"
#include "VPlatform.h"
#include "VFile.h"

#if VPLATFORM_WIN32
//////////////////////////////////////////////////////////////////////////
// Win32 Platform
//////////////////////////////////////////////////////////////////////////
DOUBLE vg_SecondsPerCycle;
UINT64 vg_AppStartCycle;

HINSTANCE hInstance = NULL; 

BOOL VPlatformInit()
{
	static BOOL Inited = FALSE;
	if (Inited)
	{
		return TRUE;
	}
	// Init timer
	{
		LARGE_INTEGER Frequency;
		LARGE_INTEGER StartTime;
		QueryPerformanceFrequency(&Frequency);
		QueryPerformanceCounter(&StartTime);
		vg_SecondsPerCycle = 1.0 / (double)Frequency.QuadPart;
		vg_AppStartCycle = StartTime.QuadPart;
	}


	Inited = TRUE;
	return TRUE;
}

void VGetSystemTime(VSystemTime& SystemTime)
{
	SYSTEMTIME st;
	GetLocalTime( &st );
	SystemTime.Year		= st.wYear;
	SystemTime.Month		= st.wMonth;
	SystemTime.DayOfWeek	= st.wDayOfWeek;
	SystemTime.Day			= st.wDay;
	SystemTime.Hour		= st.wHour;
	SystemTime.Min			= st.wMinute;
	SystemTime.Sec			= st.wSecond;
	SystemTime.MSec		= st.wMilliseconds;
}
#elif VPLATFORM_ANDROID
//////////////////////////////////////////////////////////////////////////
// Android 
//////////////////////////////////////////////////////////////////////////
DOUBLE vg_SecondsPerCycle = 1.0 / 1000000.0;
UINT64 vg_AppStartCycle;
BOOL VPlatformInit()
{
	static BOOL Inited = FALSE;
	if (Inited)
	{
		return TRUE;
	}
	// Init timer
	{
		vg_AppStartCycle = VGetCycles();
	}

	return TRUE;
}

void VGetSystemTime(VSystemTime& SystemTime)
{
	// query for calendar time
	struct timeval Time;
	gettimeofday(&Time, NULL);

	// convert it to local time
	struct tm LocalTime;
	localtime_r(&Time.tv_sec, &LocalTime);

	// pull out data/time
	SystemTime.Year		= LocalTime.tm_year + 1900;
	SystemTime.Month		= LocalTime.tm_mon + 1;
	SystemTime.DayOfWeek	= LocalTime.tm_wday;
	SystemTime.Day			= LocalTime.tm_mday;
	SystemTime.Hour		= LocalTime.tm_hour;
	SystemTime.Min			= LocalTime.tm_min;
	SystemTime.Sec			= LocalTime.tm_sec;
	SystemTime.MSec		= Time.tv_usec / 1000;
}

#elif VPLATFORM_IOS
//////////////////////////////////////////////////////////////////////////
// IOS
//////////////////////////////////////////////////////////////////////////
DOUBLE vg_SecondsPerCycle;
UINT64 vg_AppStartCycle;
BOOL VPlatformInit()
{
	static BOOL Inited = FALSE;
	if (Inited)
	{
		return TRUE;
	}
	// Init timer
	{
		mach_timebase_info_data_t Info;
		if(mach_timebase_info( &Info ) == 0 )
		{
			return FALSE;
		}
		vg_SecondsPerCycle = 1e-9 * (DOUBLE) Info.numer / (DOUBLE) Info.denom;
		vg_AppStartCycle = VGetCycles();
	}

	return TRUE;
}
#endif 

