#pragma once
#include "KFileSystem.h"

class KFileSystemWin32 : public KFileSystem, public VAllocator
{
public:
	KFileSystemWin32(void);
	~KFileSystemWin32(void);

	virtual VFile* OpenFile(const char* FileName, EVFileOpenMode FileOpenMode = VFOM_READ, UINT Flags = 0);
};

