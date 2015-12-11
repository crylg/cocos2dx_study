#ifndef __VCONFIG_H__
#define __VCONFIG_H__
#include "VString.h"

// handle *.cfg(ini) files.
// 
class VCoreApi VConfig
{
	struct Item
	{	
		VString NameStr;
		VString ValueStr;

	};
public:
	VConfig(void);
	~VConfig(void);

	BOOL Load(const char* FileName);
	void Save(const char* FileName);

};

#endif 

