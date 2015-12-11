#include "VStdAfx.h"
#include "VString.h"
#include "VThreading.h"
#include "KCorePrivate.h"


#define KSTRING_BUCKET_INIT_SIZE 2048

#if VPLATFORM_MOBILE
#define KSTRING_MEMORY_POOL_SIZE 32*1024

#else 
#define KSTRING_MEMORY_POOL_SIZE 64*1024
#endif 

// convert upper char to lower, convert '\\' to '/'
const BYTE gc_LowerTable[256] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x5B,0x2F,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
	0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
	0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
	0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
	0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
	0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
	0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF};

#if VPLATFORM_WIN32

class KStringTable
{
	struct Entry
	{
		char* Value;
		Entry* Next;
	};
	Entry**			HashTable;
	VPageMemoryPool MemoryPool;
	UINT			m_NumBuckets;
	UINT			m_ItemCount;
	BOOL			m_Inited;
	mutable volatile int	m_Lock;

	// 大小写无关的hash算法.
	static inline UINT HashString(const char* str)
	{
		UINT ret = 0;
		char c;
		while((c = *str++) != 0) 
		{
			ret <<= 1;
			ret ^= gc_LowerTable[c];
		}
		return ret;
	}

	inline UINT HashIndex(const char* str) const
	{
		UINT HashValue = HashString(str);
		return HashValue % m_NumBuckets;
	}

	static inline BOOL _StrICmp(const char* dest, const char* src)
	{
		BYTE cd,cs;
		do 
		{
			cd = gc_LowerTable[*dest++];
			cs = gc_LowerTable[*src++];
		} while (cd && (cd == cs));
		return cd == cs;
	}

	void Resize(UINT NewSize)
	{
		Entry* head = NULL, *walk, *temp;
		UINT i;
		//将哈希表中所有桶的链表串联起来.
		for(i = 0; i < m_NumBuckets; i++) 
		{
			walk = HashTable[i];
			while(walk)
			{
				temp = walk->Next;
				walk->Next = head;
				head = walk;
				walk = temp;
			}
		}
		// 重新分配桶
		HashTable = (Entry**)VRealloc(HashTable, NewSize * sizeof(Entry));
		memset(HashTable, 0, NewSize * sizeof(Entry));
		m_NumBuckets = NewSize;

		// 将串联起来的所有数据重新哈希
		UINT key;
		walk = head;
		while(walk) 
		{
			temp = walk;
			walk = walk->Next;
			key = HashIndex(temp->Value);
			temp->Next = HashTable[key];
			HashTable[key] = temp;
		}
	}
public:
	KStringTable():HashTable(NULL),MemoryPool(KSTRING_MEMORY_POOL_SIZE),m_NumBuckets(0)
		,m_ItemCount(0),m_Inited(FALSE),m_Lock(0)
	{
	}

	BOOL Init(UINT Buckets = KSTRING_BUCKET_INIT_SIZE)
	{
		if (m_Inited)
		{
			return TRUE;
		}
		HashTable = (Entry**)VMalloc(sizeof(Entry*) * Buckets);
		memset(HashTable, 0, sizeof(Entry*)*Buckets);
		m_NumBuckets = Buckets;
		m_ItemCount = 0;
		m_Inited = TRUE;
		return TRUE;
	}

	void Shutdown()
	{
		VSyncSpinLock lock(&m_Lock);
		if (HashTable)
		{
			VFree(HashTable);
			HashTable = NULL;
		}
		MemoryPool.FreePool();
	}

	const char* AddString(const char *Str)
	{
		if (!m_Inited)
		{
			Init();
		}
		UINT Key = HashIndex(Str);
		Entry **walk, *temp;

		// lock
		VSyncSpinLock lock(&m_Lock);
		walk = &HashTable[Key];

		// 是否已经存在?
		while((temp = *walk) != NULL)
		{
			if(_StrICmp(temp->Value, Str))
			{
				return temp->Value;
			}
			walk = &(temp->Next);
		}

		// 加入
		char *ret = 0;
		if(*walk == NULL) 
		{
			*walk = (Entry*)MemoryPool.Alloc(sizeof(Entry));
			(*walk)->Next = NULL;
			int Len = strlen(Str);
			(*walk)->Value = (char*)MemoryPool.Alloc(Len + 1);
			VStrcpy((*walk)->Value, Len + 1,Str);
			ret = (*walk)->Value;
			m_ItemCount++;
		}
		if (m_ItemCount > (2*m_NumBuckets))
		{
			Resize(m_NumBuckets * 2);
		}
		return ret;
	}

	const char* Find(const char *Str) const
	{
		VSyncSpinLock lock(&m_Lock);
		UINT Key = HashIndex(Str);
		Entry* E = HashTable[Key];
		for (; E != NULL; E = E->Next)
		{
			if (_StrICmp(E->Value,Str))
			{
				return E->Value;
			}
		}
		return NULL;
	}
};

KStringTable* g_RefStringTable = NULL;

void KStringTableInit()
{
	if (g_RefStringTable == NULL)
	{
		g_RefStringTable = new KStringTable;
		g_RefStringTable->Init();
	}
	
}

void KStringTableShutdown()
{
	if (g_RefStringTable)
	{
		g_RefStringTable->Shutdown();
		delete g_RefStringTable;
		g_RefStringTable = NULL;
	}
}

const char* VRefString::AllocReference(const char* Str)
{
	VASSERT(g_RefStringTable);
	return g_RefStringTable->AddString(Str);
}

void VRefString::DumpInfo()
{

}

UINT VRefString::MemUsage()
{
	return 0;
}

#endif 