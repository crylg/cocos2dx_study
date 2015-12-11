#pragma once
#ifndef __VLOG_H__
#define __VLOG_H__

#define MAX_LOG_MESSAGE 2048

enum EVLogLevel
{
	LOG_MESSAGE,
	LOG_DEBUG,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FILE
};

class VLogDevice
{
public:
	virtual void Printf(const char* sMessage, EVLogLevel LogLevel) = 0;
};



class VCoreApi VLogger
{
public:
	static void SetLogDevice(VLogDevice* LogDevice);
	static void Message(const char *format,...);
	static void Debug(const char *format, ...);
	static void Warning(const char *format,...);
	static void Error(const char *format,...);
	static void File(const char *format,...);
};

#if VDEBUG
#define VLOG_DEBUG VLogger::Debug
#else 
#define VLOG_DEBUG __noop
#endif 

#define VLOG VLogger::Message
#define VLOG_WARN VLogger::Warning
#define VLOG_ERROR VLogger::Error
#define VLOG_FILE VLogger::File




#endif 