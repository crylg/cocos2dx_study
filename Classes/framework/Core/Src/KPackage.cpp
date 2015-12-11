#include "VStdAfx.h"
#include "KPackage.h"


class KPakFile : public VFile
{
	KPackage* m_Package;
	const KPAKFileEntry* m_FileEntry;




	KPakFile()
	{

	}
};


KPackage::KPackage(void)
{
}

KPackage::~KPackage(void)
{
}

BOOL KPackage::FileExist(const char* sFileName) const
{
	return FALSE;
}