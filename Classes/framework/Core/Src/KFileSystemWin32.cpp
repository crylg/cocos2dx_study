#include "VStdAfx.h"
#if VPLATFORM_WIN32
#include "KFileSystemWin32.h"


class KFileWin32 : public VFile , public VAllocator
{
	HANDLE m_hFile;

	virtual void Close()
	{
		if (m_hFile)
		{
			CloseHandle(m_hFile); m_hFile = NULL;
		}
		VDelete this;
	}
	virtual UINT Size()
	{
		VASSERT(m_hFile);
		return (UINT)GetFileSize(m_hFile, NULL);
	}
	virtual BOOL Seek(UINT Pos)
	{
		VASSERT(m_hFile);
		if( SetFilePointer( m_hFile, Pos, NULL, FILE_BEGIN )==INVALID_SET_FILE_POINTER )
		{
			return FALSE;
		}
		return TRUE;
	}
	virtual UINT Tell() const
	{
		return SetFilePointer( m_hFile, 0, NULL, FILE_CURRENT );
	}
	virtual UINT Read(void* Dest, UINT Size)
	{
		VASSERT(m_hFile);
		DWORD NumReaded = 0;
		ReadFile(m_hFile, Dest, (DWORD)Size, &NumReaded, NULL);
		return NumReaded;	
	}
	virtual UINT Write(const void* Dest, UINT Size)
	{
		VASSERT(m_hFile);
		DWORD NumWrited = 0;
		WriteFile(m_hFile, Dest, Size, &NumWrited, NULL);
		return (UINT)NumWrited;
	}
	virtual void Flush()
	{

	}
public:
	KFileWin32(HANDLE hFile):m_hFile(hFile)
	{

	}
	virtual ~KFileWin32()
	{

	}
};


KFileSystemWin32::KFileSystemWin32(void)
{
}


KFileSystemWin32::~KFileSystemWin32(void)
{
}

VFile* KFileSystemWin32::OpenFile(const char* FileName, EVFileOpenMode FileOpenMode, UINT Flags)
{
	if (FileOpenMode == VFOM_READ)
	{
		HANDLE Handle    = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( Handle==INVALID_HANDLE_VALUE )
		{
			return NULL;
		}
		KFileWin32* File = VNew KFileWin32(Handle);
		return File;
	}else
	{
		// 
		//MakeDirectory(*FFilename(Filename).GetPath(), TRUE);

		//if( (FileSize (Filename) >= 0) && (Flags & FILEWRITE_EvenIfReadOnly) )
		//{
		//	SetFileAttributesW(Filename, 0);
		//}
		DWORD  Access    = GENERIC_WRITE;
		//DWORD  WinFlags  = (Flags & FILEWRITE_AllowRead) ? FILE_SHARE_READ : 0;
		//DWORD  Create    = (Flags & FILEWRITE_Append) ? OPEN_ALWAYS : (Flags & FILEWRITE_NoReplaceExisting) ? CREATE_NEW : CREATE_ALWAYS;
		HANDLE Handle    = CreateFileA(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		INT    Pos       = 0;
		if( Handle==INVALID_HANDLE_VALUE )
		{
			return NULL;
		}
		//if( Flags & FILEWRITE_Append )
		//{
			//Pos = SetFilePointer( Handle, 0, NULL, FILE_END );
		//}
		KFileWin32* File = VNew KFileWin32(Handle);
		return File;
	}
	return NULL;
}

#endif 