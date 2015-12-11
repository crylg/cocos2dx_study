#include "VStdAfx.h"
#include "VLog.h"
#if VPLATFORM_ANDROID
#include <jni.h>
#include <android/log.h>
#endif 

static FILE *file = NULL;
static volatile int lock = 0;


class DefaultLogDevice : public VLogDevice
{
	virtual void Printf(const char* sMessage, EVLogLevel LogLevel) override
	{
#if VPLATFORM_ANDROID
		__android_log_print(ANDROID_LOG_DEBUG,"VFramework", "%s",sMessage);
#elif VPLATFORM_WIN32
		static char szBuffer[MAX_LOG_MESSAGE];
		sprintf_s(szBuffer, "%s\n", sMessage);
		OutputDebugStringA(szBuffer);
#endif 
	}
}g_DefaultLogDevice;

VLogDevice* LogDevice = &g_DefaultLogDevice;
static char szBuffer[MAX_LOG_MESSAGE] = {};

void VLogger::SetLogDevice(VLogDevice* _LogDevice)
{
	VSyncSpinLock atomic(&lock);

	if (_LogDevice)
	{
		LogDevice = _LogDevice;
	}
	else
	{
		LogDevice = &g_DefaultLogDevice;
	}
}

void VLogger::Message(const char *format,...)
{
	VSyncSpinLock atomic(&lock);
	
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(szBuffer, sizeof(szBuffer), format, argptr);
	va_end(argptr);
	LogDevice->Printf(szBuffer, EVLogLevel::LOG_MESSAGE);
}

void VLogger::Debug(const char *format, ...)
{
	VSyncSpinLock atomic(&lock);

	va_list argptr;
	va_start(argptr, format);
	vsnprintf(szBuffer, sizeof(szBuffer), format, argptr);
	va_end(argptr);
	LogDevice->Printf(szBuffer, EVLogLevel::LOG_DEBUG);
}

// warning
void VLogger::Warning(const char *format,...)
{
	VSyncSpinLock atomic(&lock);
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(szBuffer, sizeof(szBuffer), format, argptr);
	va_end(argptr);
	LogDevice->Printf(szBuffer, EVLogLevel::LOG_WARNING);
}

// error
void VLogger::Error(const char *format,...)
{
	VSyncSpinLock atomic(&lock);
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(szBuffer, sizeof(szBuffer), format, argptr);
	va_end(argptr);
	LogDevice->Printf(szBuffer, EVLogLevel::LOG_ERROR);
}

void VLogger::File(const char *format, ...)
{
	VSyncSpinLock atomic(&lock);
	va_list argptr;
	va_start(argptr, format);
	vsnprintf(szBuffer, sizeof(szBuffer), format, argptr);
	va_end(argptr);
	LogDevice->Printf(szBuffer, EVLogLevel::LOG_FILE);
}


