#ifndef __KFILESYSTEM_H__
#define __KFILESYSTEM_H__
#include "VFile.h"

// 实现Package相关的接口
class KFileSystem : public IVFileSystem
{
public:

	struct PackageFile
	{

	};

	KFileSystem(void);
	virtual ~KFileSystem(void);

	// 打开读取文件流
	virtual VReadFileStream* OpenReadFileStream(const char* FileName)
	{
		VFile* File = OpenFile(FileName, VFOM_READ);
		if (File)
		{
			VReadFileStream* ReadStream = VNew VReadFileStream(File);
			return ReadStream;
		}
		return NULL;
	}

	// 打开写入文件流
	virtual VWriteFileStream* OpenWriteFileStream(const char* FileName)
	{
		return NULL;
	}

	virtual void Shutdown() { VDelete this; }
	
	virtual UINT AsyncReadFile(const char* FileName, void*)
	{
		return 0;
	}
	virtual void CancelAsyncRequest(UINT* Requests, UINT NumRequest)
	{

	}
protected:
	vtpl::map<UINT,PackageFile*> m_PackageFiles;

};

BOOL KFileSystemInit();
void KFileSystemShutdown();

#endif 