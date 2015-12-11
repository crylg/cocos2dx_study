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
	UINT ID;			// �ļ���ʶ                         		  
	UINT HashTablePos;	// Entry Table λ��	
	UINT HashTableSize;
	UINT FileMapPos;	// �ļ���ӳ���ƫ��  0xFFFFFFF ��ʶPAK�в��������ļ���ӳ���.	
	UINT FileMapSize;	// �ļ���ӳ����С(Bytes)
};

struct KPAKFileEntry
{
	UINT	Name1;			// �ļ��� 1
	UINT	Name2;			// �ļ��� 2
	UINT	FilePos;		// �ļ�λ��	
	UINT	CSize;			// ѹ����С
	UINT	FSize;			// �������ļ���Ч, ԭʼ��С.      (�ļ��е�,��СΪ0)                 
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
	WORD	Length;                     // �ļ�������
	char	szFileName[1];                // �ļ���
};

#pragma pack(pop)





// �����ļ���hash. (������2��hash�㷨�������ļ�����ͻ.)



class KPackage : public VPackage
{
public:
	KPackage(void);
	~KPackage(void);


	BOOL FileExist(const char* sFileName) const;

	// �����ļ���CRC32ֵ.(��Сд�޹�)
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
