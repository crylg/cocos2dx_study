#include "VStdAfx.h"
#include "KFileSystem.h"
#include "KFileSystemWin32.h"

IVFileSystem* VFileSystem = NULL;


BOOL KFileSystemInit()
{

	VPath::Init();

#if VPLATFORM_WIN32
	VFileSystem = VNew KFileSystemWin32;
#else 
#endif 
	return VFileSystem != NULL;
}

void KFileSystemShutdown()
{
	if (VFileSystem)
	{
		((KFileSystem*)VFileSystem)->Shutdown();
		VFileSystem = NULL;
	}

	VPath::Shutdown();
}

KFileSystem::KFileSystem(void)
{
}


KFileSystem::~KFileSystem(void)
{
}

