#pragma once
#include "KFileSystem.h"


class KFileStdio : public VFile
{
	FILE* m_hFile;

	virtual void Close()
	{
		if (m_hFile)
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
		VDelete this;
	}
	virtual UINT Size()
	{
		VASSERT(m_hFile);
		UINT Offset = (UINT)ftell(m_hFile);
		fseek(m_hFile, 0, SEEK_END);
		UINT size = (UINT)ftell(m_hFile);
		fseek(m_hFile, Offset, SEEK_SET);
		return size;
	}
	virtual UINT Tell() const
	{
		VASSERT(m_hFile);
		return (UINT)ftell(m_hFile);
	}
	virtual BOOL Seek(UINT Pos)
	{
		VASSERT(m_hFile);
		return fseek(m_hFile, Pos, SEEK_CUR) == 0;
	}
	virtual UINT Read(void* Dest, UINT Size)
	{
		VASSERT(m_hFile);

	}
	virtual UINT Write(const void* Dest, UINT Size)
	{
		VASSERT(m_hFile);
	}
	virtual void Flush()
	{
		VASSERT(m_hFile);
		fflush(m_hFile);
	}
public:
	inline KFileStdio(FILE* hFile):m_hFile(hFile){}
	virtual ~KFileStdio()
	{
	}
};

class KFileSystemSTD : public KFileSystem
{
public:
	KFileSystemSTD(void);
	~KFileSystemSTD(void);
};

