#include "VStdAfx.h"
#include "VViewManager.h"


VViewManager VViewManager::mInstance;

VViewManager::VViewManager()
{
}


VViewManager::~VViewManager()
{
}

VViewManager* VViewManager::GetInstance()
{
//	static VViewManager instance;
	return &mInstance;
}
