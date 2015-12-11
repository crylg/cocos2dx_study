/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2014 Venus.PK, All Right Reversed.
//	VFile.h
//	Created: 2014-2-12   11:20
// Venus now use unix path internal.
**********************************************************************************/
#ifndef __VFILE_H__
#define __VFILE_H__
#include "VString.h"
#include "VStream.h"

inline void VPathToUnix(char* Path)
{
	while (*Path) {
		if (*Path == '\\')
			*Path = '/';
		++Path;
	}
}

inline void VPathToDos(char* Path)
{
	while (*Path) {
		if (*Path == '/')
			*Path = '\\';
		++Path;
	}
}
VCoreApi extern const BYTE vc_PathLowerTable[256];
VCoreApi extern const UINT vc_CRCTable[256];
// fast convert path to lower and convert '\\' to unix slash '/'
inline void VPathLower(char* Path)
{
	while (*Path) {
		*Path = (char)vc_PathLowerTable[*Path];
		++Path;
	}
}

inline UINT VPathHash(const char* Path)
{
	UINT CRC = 0xFFFFFFFF;
	UINT Char;
	while(*Path)
	{
		// Convert to lower 
		Char = (UINT)vc_PathLowerTable[*Path++];
		// CRC
		CRC  = (CRC << 8) ^ vc_CRCTable[(CRC >> 24) ^Char];
	}
	return ~CRC;
}

// get .ext from "dir/../subdir/filename.ext"
VCoreApi VString VPathExtension(const char* FileName, BOOL IncludeDot = FALSE);
VCoreApi VString VPathFileName(const char* FileName);

VCoreApi VString VPathToFullPath(const char* RelativePath);


class VCoreApi VPath
{
public:
	/**
	 *  ���Ӧ�ó������ڵľ���·��, .exe, 
	 *
	 * @param 
	 */
	static inline const VString& AppPath() { return vAppPath; }

	/**
	 *  ���Ӧ�ó�����Դ·�� 
	 *
	 * @param 
	 */
	static const VString& DataPath() { return vDataPath; }


	/**
	 * ��·�����Ƴ�����ļ���. ./ .//  . ���� convert "./base_dir/sub_dir/../texture/" --> "base_dir/texture/"
	 *
	 * @param PathHasRelativeDirectory ��������ļ��е�Unix·��
	 */
	static BOOL RemoveRelativeDirectory(VString& PathHasRelativeDirectory);

	static VString RemoveRelativeDirectory(const char* PathHasRelativeDirectory);


	/**
	 *  �滻·����չ��. 
	 *
	 * @param NewExtension �µ���չ��, ����Ϊ��. 
	 */
	static void ReplaceExtension(VString& Path, const char* NewExtension);

	static void AddSlash(VString& Path);

	static void Init();
	static void Shutdown();
private:
	static VString vAppPath;
	static VString vDataPath;
};



class VFile
{
public:
	// Close file and destroy this.
	virtual void Close() = 0;
	virtual UINT Size() = 0;
	virtual BOOL Seek(UINT Pos) = 0;
	virtual UINT Tell() const = 0;
	virtual UINT Read(void* Dest, UINT Size) = 0;
	virtual UINT Write(const void* Dest, UINT Size) = 0;
	virtual void Flush() = 0;
};


// Bufferred File Read Stream
class VCoreApi VReadFileStream : public VStream , public VAllocator
{
	enum 
	{
		BUFFER_SIZE = 1024,
	};
public:
	VReadFileStream();
	VReadFileStream(VFile* File);
	virtual ~VReadFileStream();

	BOOL Open(const char* FileName);

	virtual void Release();
	virtual void Close();
	virtual UINT Size();
	virtual BOOL Seek(UINT Pos);
	virtual UINT Read(void* Dest, UINT Size);
	virtual UINT Write(const void* Dest, UINT Size);
	virtual UINT Tell() const;
	virtual void Flush();
protected:
	VFile*	m_File;
	UINT	m_Size;
	UINT	m_CurPos;
	UINT	m_BufferBase;
	UINT	m_BufferCount;
	BYTE	m_Buffer[BUFFER_SIZE];
};

// Bufferred File Write Stream
class VCoreApi VWriteFileStream : public VStream
{
	enum 
	{
		BUFFER_SIZE = 4096,
	};
public:
	VWriteFileStream();
	VWriteFileStream(VFile* File);
	~VWriteFileStream();

	BOOL Open(const char* FileName);

	virtual void Close();
	virtual UINT Size();
	virtual BOOL Seek(UINT Pos);
	virtual UINT Read(void* Dest, UINT Size);
	virtual UINT Write(const void* Dest, UINT Size);
	virtual UINT Tell() const;
	virtual void Flush();
protected:
	VFile*	m_File;
	UINT	m_CurPos;
	UINT	m_BufferCount;
	BYTE	m_Buffer[BUFFER_SIZE];
};


class VPackage
{
public:
	virtual void Close() = 0;
	virtual UINT NumFiles() const  = 0;
	//virtual VFileStream* OpenFile(const char* FileName) = 0;
	virtual UINT FindFile(const char* FileName) const = 0;
};


enum EVFileOpenMode
{
	VFOM_READ = 1,
	VFOM_WRITE = 2,
};

enum EVFileOpenPriority
{
	VFOP_PACKAGE_ONLY = 0,
	VFOP_DISK_ONLY,
	VFOP_PACKAGE_FIRST,

};

class IVFileSystem
{
public:
	// ���ļ�����
	virtual VFile* OpenFile(const char* FileName, EVFileOpenMode FileOpenMode = VFOM_READ, UINT Flags = 0) = 0;

	// �򿪶�ȡ�ļ���
	virtual VReadFileStream* OpenReadFileStream(const char* FileName) = 0;

	// ��д���ļ���
	virtual VWriteFileStream* OpenWriteFileStream(const char* FileName) = 0;

	VPackage* OpenPackage(const char* PakageName);

	// ��ĳ��Ŀ¼ӳ�䵽package ��. 
	BOOL MapPackage(const char* Dir, VPackage* Pak);

	BOOL DeleteFile(const char* FileName);

	// Async IO
	virtual UINT AsyncReadFile(const char* FileName, void*) = 0;
	virtual void CancelAsyncRequest(UINT* Requests, UINT NumRequest) = 0;
};


// FileSystem (VCoreInit �ɹ�����Ч.)
VCoreApi extern IVFileSystem* VFileSystem;


#endif 
