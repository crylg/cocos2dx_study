/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2014 Venus.PK, All Right Reversed.
//	KPackage.h
//	Created: 2009-4-12   12:34
//
**********************************************************************************/
#ifndef __KPACKAGE_H__
#define __KPACKAGE_H__
#pragma once
#include "VFile.h"

#pragma pack(push)
#pragma pack(1)

struct KPAKHeader
{
	UINT ID;			// 文件标识                         		  
	UINT HashTablePos;	// Entry Table 位置	
	UINT HashTableSize;
	UINT FileMapPos;	// 文件名映射表偏移  0xFFFFFFF 标识PAK中并不包含文件名映射表.	
	UINT FileMapSize;	// 文件名映射表大小(Bytes)
};

struct KPAKFileEntry
{
	UINT	Name1;			// 文件名 1
	UINT	Name2;			// 文件名 2
	UINT	FilePos;		// 文件位置	
	UINT	CSize;			// 压缩大小
	UINT	FSize;			// 仅仅对文件有效, 原始大小.      (文件夹等,大小为0)                 
};

struct KPAKFileItem
{
	UINT				FSize;
	UINT				CSize;
	UINT				RawFilePos;	
	char				szFileName[1];			// File name (variable length)
};

struct KPAKFileMapItem
{
	WORD	Length;                     // 文件名长度
	char	szFileName[1];                // 文件名
};

#pragma pack(pop)





// 返回文件名hash. (我们用2套hash算法来避免文件名冲突.)



class KPackage : public VPackage
{
public:
	KPackage(void);
	~KPackage(void);


	BOOL FileExist(const char* sFileName) const;

	// 返回文件名CRC32值.(大小写无关)
	static inline UINT FileNameCRC(const char* sFileName)
	{
		DWORD CRC = 0xFFFFFFFF;
		CHAR chr;
		while((chr = *sFileName++) != 0) 
		{
			//CRC = (CRC << 8) ^ vc_CRC32Table[(CRC >> 24) ^ gc_LowerTable[chr]];
		}
		return ~CRC;
	}

	static inline UINT HashFileName(const char* szFileName)
	{
		DWORD  CRC = 0;
		DWORD  ch;
		while((ch = *szFileName++) != 0)
		{
			//ch = gc_LowerTable[ch];
			//CRC = (CRC << 8) ^ c_CRC32HashTable2[(CRC >> 24) ^ ch];
		}
		return CRC;
	}

private:
	KPAKFileEntry*	m_FileEntryTable;

};

#endif 
