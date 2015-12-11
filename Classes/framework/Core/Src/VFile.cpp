#include "VStdAfx.h"
#include "VFile.h"
#include "KFileSystemWin32.h"


// convert upper char to lower, convert '\\' to '/'
const BYTE vc_PathLowerTable[256] = {
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

/*


//#define CRC32_POLY 0x06F38BAD
#define CRC32_POLY 0x06F38BAD
void MakeCRCTable()
{
for (n = 0; n < 256; n++)
{
	c = n << 24;
	for (k = 0; k < 8; k++)
		c = c & 0x80000000 ? CRC32_POLY ^ (c << 1) : c << 1;
	CRCTable[n] = c;
}
}
int _tmain(int argc, _TCHAR* argv[])
{
	MakeCRCTable();
	FILE* fp = fopen("crc.txt", "wt+");
	for (UINT I = 0; I < 256; I++)
	{
		fprintf(fp, "0x%08XL,", CRCTable[I]);
	}
	fclose(fp);
	return 0;
}
*/

const UINT vc_CRCTable[256] = {
0x00000000L,0x06F38BADL,0x0DE7175AL,0x0B149CF7L,0x1BCE2EB4L,0x1D3DA519L,0x162939EEL,0x10DAB243L,
0x379C5D68L,0x316FD6C5L,0x3A7B4A32L,0x3C88C19FL,0x2C5273DCL,0x2AA1F871L,0x21B56486L,0x2746EF2BL,
0x6F38BAD0L,0x69CB317DL,0x62DFAD8AL,0x642C2627L,0x74F69464L,0x72051FC9L,0x7911833EL,0x7FE20893L,
0x58A4E7B8L,0x5E576C15L,0x5543F0E2L,0x53B07B4FL,0x436AC90CL,0x459942A1L,0x4E8DDE56L,0x487E55FBL,
0xDE7175A0L,0xD882FE0DL,0xD39662FAL,0xD565E957L,0xC5BF5B14L,0xC34CD0B9L,0xC8584C4EL,0xCEABC7E3L,
0xE9ED28C8L,0xEF1EA365L,0xE40A3F92L,0xE2F9B43FL,0xF223067CL,0xF4D08DD1L,0xFFC41126L,0xF9379A8BL,
0xB149CF70L,0xB7BA44DDL,0xBCAED82AL,0xBA5D5387L,0xAA87E1C4L,0xAC746A69L,0xA760F69EL,0xA1937D33L,
0x86D59218L,0x802619B5L,0x8B328542L,0x8DC10EEFL,0x9D1BBCACL,0x9BE83701L,0x90FCABF6L,0x960F205BL,
0xBA1160EDL,0xBCE2EB40L,0xB7F677B7L,0xB105FC1AL,0xA1DF4E59L,0xA72CC5F4L,0xAC385903L,0xAACBD2AEL,
0x8D8D3D85L,0x8B7EB628L,0x806A2ADFL,0x8699A172L,0x96431331L,0x90B0989CL,0x9BA4046BL,0x9D578FC6L,
0xD529DA3DL,0xD3DA5190L,0xD8CECD67L,0xDE3D46CAL,0xCEE7F489L,0xC8147F24L,0xC300E3D3L,0xC5F3687EL,
0xE2B58755L,0xE4460CF8L,0xEF52900FL,0xE9A11BA2L,0xF97BA9E1L,0xFF88224CL,0xF49CBEBBL,0xF26F3516L,
0x6460154DL,0x62939EE0L,0x69870217L,0x6F7489BAL,0x7FAE3BF9L,0x795DB054L,0x72492CA3L,0x74BAA70EL,
0x53FC4825L,0x550FC388L,0x5E1B5F7FL,0x58E8D4D2L,0x48326691L,0x4EC1ED3CL,0x45D571CBL,0x4326FA66L,
0x0B58AF9DL,0x0DAB2430L,0x06BFB8C7L,0x004C336AL,0x10968129L,0x16650A84L,0x1D719673L,0x1B821DDEL,
0x3CC4F2F5L,0x3A377958L,0x3123E5AFL,0x37D06E02L,0x270ADC41L,0x21F957ECL,0x2AEDCB1BL,0x2C1E40B6L,
0x72D14A77L,0x7422C1DAL,0x7F365D2DL,0x79C5D680L,0x691F64C3L,0x6FECEF6EL,0x64F87399L,0x620BF834L,
0x454D171FL,0x43BE9CB2L,0x48AA0045L,0x4E598BE8L,0x5E8339ABL,0x5870B206L,0x53642EF1L,0x5597A55CL,
0x1DE9F0A7L,0x1B1A7B0AL,0x100EE7FDL,0x16FD6C50L,0x0627DE13L,0x00D455BEL,0x0BC0C949L,0x0D3342E4L,
0x2A75ADCFL,0x2C862662L,0x2792BA95L,0x21613138L,0x31BB837BL,0x374808D6L,0x3C5C9421L,0x3AAF1F8CL,
0xACA03FD7L,0xAA53B47AL,0xA147288DL,0xA7B4A320L,0xB76E1163L,0xB19D9ACEL,0xBA890639L,0xBC7A8D94L,
0x9B3C62BFL,0x9DCFE912L,0x96DB75E5L,0x9028FE48L,0x80F24C0BL,0x8601C7A6L,0x8D155B51L,0x8BE6D0FCL,
0xC3988507L,0xC56B0EAAL,0xCE7F925DL,0xC88C19F0L,0xD856ABB3L,0xDEA5201EL,0xD5B1BCE9L,0xD3423744L,
0xF404D86FL,0xF2F753C2L,0xF9E3CF35L,0xFF104498L,0xEFCAF6DBL,0xE9397D76L,0xE22DE181L,0xE4DE6A2CL,
0xC8C02A9AL,0xCE33A137L,0xC5273DC0L,0xC3D4B66DL,0xD30E042EL,0xD5FD8F83L,0xDEE91374L,0xD81A98D9L,
0xFF5C77F2L,0xF9AFFC5FL,0xF2BB60A8L,0xF448EB05L,0xE4925946L,0xE261D2EBL,0xE9754E1CL,0xEF86C5B1L,
0xA7F8904AL,0xA10B1BE7L,0xAA1F8710L,0xACEC0CBDL,0xBC36BEFEL,0xBAC53553L,0xB1D1A9A4L,0xB7222209L,
0x9064CD22L,0x9697468FL,0x9D83DA78L,0x9B7051D5L,0x8BAAE396L,0x8D59683BL,0x864DF4CCL,0x80BE7F61L,
0x16B15F3AL,0x1042D497L,0x1B564860L,0x1DA5C3CDL,0x0D7F718EL,0x0B8CFA23L,0x009866D4L,0x066BED79L,
0x212D0252L,0x27DE89FFL,0x2CCA1508L,0x2A399EA5L,0x3AE32CE6L,0x3C10A74BL,0x37043BBCL,0x31F7B011L,
0x7989E5EAL,0x7F7A6E47L,0x746EF2B0L,0x729D791DL,0x6247CB5EL,0x64B440F3L,0x6FA0DC04L,0x695357A9L,
0x4E15B882L,0x48E6332FL,0x43F2AFD8L,0x45012475L,0x55DB9636L,0x53281D9BL,0x583C816CL,0x5ECF0AC1L
};



VString VPathExtension(const char* FileName, BOOL IncludeDot)
{
	const char* Dot = strrchr(FileName, '.');
	if (Dot != NULL)
	{
		if (IncludeDot)
		{
			Dot--;
		}
		return VString(Dot);
	}
	return VString(0);
}

VString VPathFileName(const char* FileName)
{
	return VString(0);
}

VString VPathToFullPath(const char* RelativePath)
{
	return VString(RelativePath);
}


VFile* VFileOpen(const char* FileName, EVFileOpenMode FileOpenMode)
{
#if VPLATFORM_WIN32

	//GetModuleFileName
	return NULL;

#elif VPLATFORM_ANDROID || VPLATFORM_IOS


#else 
	return NULL;
#endif	
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

VString VPath::vAppPath;
VString VPath::vDataPath;



#if VPLATFORM_WIN32
VString GetAppPath()
{
	VString AppPath;
	AppPath.Increment(512, 0);
	char* cAppPath = &AppPath[0];

	GetModuleFileNameA( hInstance, cAppPath, 512);

	// 将'\\' 转换为'/' 我们依照UNIX路径规范. 
	VPathToUnix(cAppPath);


	// 将 Path/App.exe 转换为路径  Path/ 
	char* LastSlash = strrchr(cAppPath, '/');
	VASSERT(LastSlash);
	*(LastSlash+1) = 0;
	VString AbsPath(cAppPath);
	
	VPath::RemoveRelativeDirectory(AbsPath);
	
	return AbsPath;
}

#elif VPLATFORM_ANDROID
VString AndroidExternalPath;
VString GetAppPath()
{
	// android app path always return ""
	return VString();
}
#endif 



void VPath::Init()
{
	vAppPath = GetAppPath();


#if VPLATFORM_WIN32
	vDataPath = vAppPath + "../GameData/";
	VPath::RemoveRelativeDirectory(vDataPath);
#elif VPLATFORM_ANDROID

#endif 

	
}

void VPath::Shutdown()
{
	vDataPath.Clear();
	vAppPath.Clear();
	vDataPath.Compact();
	vAppPath.Compact();

#if VPLATFORM_ANDROID
	AndroidExternalPath.Clear(); AndroidExternalPath.Compact();
#endif
}

inline char* _StrRChr(char* Str, char Char, char* SearchOffset)
{
	char* Ret = SearchOffset;
	for ( ; Ret >= Str; --Ret)
	{
		if (*Ret == Char)
		{
			return Ret;
		}
	}
	return NULL;
}
inline char* _StrChr(char* Str, char Char, char* SearchEnd)
{
	char* Ret = Str;
	for ( ; Ret < SearchEnd; ++Ret)
	{
		if (*Ret == Char)
		{
			return Ret;
		}
	}
	return NULL;
}



BOOL VPath::RemoveRelativeDirectory(VString& Path)
{
	if (Path.Empty())
	{
		return FALSE;
	}

	const char ParentDir[] = "/..";
	const UINT ParentDirLength = 3; // To avoid hardcoded values
	char *s;
	// 首先转换/../路径. 对于./ 路径则放到最后直接替换. 
	while(1)
	{
		// An empty path is finished
		if (Path.Empty())
		{
			break;
		}

		s = Path.GetData();

		// 如果路径以.. or /..开头, 我们无法转换这个路径. 
		if (strncmp(s, "..", 2) == 0 || strncmp(s, "/..", 3) == 0)
		{
			return FALSE;
		}
		

		// 查找上级目录标识.
		char* Mark = strstr(s, ParentDir);
		if (Mark == NULL)
		{
			break;
		}
		
		// 查找上级目录的起始位置
		char* ParentPath = Mark;
		while (1)
		{
			ParentPath = _StrRChr(s, '/', ParentPath - 1);
			if (ParentPath == NULL)
			{
				// 没有找到上级目录
				ParentPath = s;
				break;
			}

			// Stop if we've found a directory that isn't "/./"
			if ((Mark - ParentPath) > 1 && ParentPath[1] != '.' && ParentPath[2] != '/')
			{
				break;
			}
		}

		// 确保没有输入这样的无效路径: E:/../Data;

		char* Colon = _StrChr(ParentPath, ':', Mark);
		if (Colon)
		{
			return FALSE;
		}

		Path.EraseAt((UINT)(ParentPath - s), (UINT)(Mark - ParentPath) + ParentDirLength);

	}

	Path.ReplaceFast("./", "", TRUE);

	/*InPath.ReplaceInline(TEXT("./"), TEXT(""));

	InPath.TrimToNullTerminator();*/
	return TRUE;
}


void VPath::ReplaceExtension(VString& Path, const char* NewExtension)
{
	if (Path.Empty())
	{
		return;
	}
	int Index = Path.RFind('.');
	if (Index == -1)
	{
		return;
	}

//	Path.EraseAt(Index, )

}

void VPath::AddSlash(VString& Path)
{
	//const char* pExt = Path
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
VReadFileStream::VReadFileStream()
	:m_File(NULL),m_Size(0),m_CurPos(0),m_BufferBase(0),m_BufferCount(0)
{
}
VReadFileStream::VReadFileStream(VFile* File)
	:m_File(File),m_Size(0),m_CurPos(0),m_BufferBase(0),m_BufferCount(0)
{
	if (m_File)
	{
		m_Size = m_File->Size();
	}
}

VReadFileStream::~VReadFileStream()
{
	if (m_File)
	{
		m_File->Close();
		m_File = NULL;
	}
}

void VReadFileStream::Release()
{
	VDelete this;
}

BOOL VReadFileStream::Open(const char* FileName)
{
	VASSERT(VFileSystem);
	Close();
	VFile* File = VFileSystem->OpenFile(FileName, VFOM_READ);
	if (File)
	{
		m_File = File;
		m_Size = File->Size();
		return TRUE;
	}
	return FALSE;
}

void VReadFileStream::Close() 
{
	if (m_File)
	{
		m_File->Close(); m_File = NULL;
	}
	m_Size = 0;
	m_CurPos = 0;
	m_BufferBase = 0;
	m_BufferCount = 0;

}

UINT VReadFileStream::Size() 
{ 
	return m_Size; 
}

BOOL VReadFileStream::Seek(UINT Pos)
{
	VASSERT(Pos>=0);
	VASSERT(Pos<=m_Size);
	VASSERT(m_File);
	if(!m_File->Seek(Pos))
	{
		m_CurPos = m_File->Tell();
		m_BufferBase = m_CurPos;
		m_BufferCount = 0;
		return FALSE;
	}
	m_CurPos       = Pos;
	m_BufferBase  = Pos;
	m_BufferCount = 0;
	return TRUE;
}

UINT VReadFileStream::Read(void* Dest, UINT Size)
{
	VASSERT(m_File);
	UINT ReadedBytes = 0;
	while( Size >0 )
	{
		UINT Copy = VMin( Size, m_BufferBase+m_BufferCount-m_CurPos );
		if( Copy==0 )
		{	// 缓存中不存在可拷贝的字节
			
			if( Size >= BUFFER_SIZE)
			{	
				// 读取的大小大于缓存容量, 直接从文件中IO
				UINT Count = m_File->Read(Dest, Size);
				ReadedBytes += Count;
				if( Count != Size )
				{
					// 文件IO发生错误或者EOF. 
					return ReadedBytes;
				}
				m_CurPos += Size;
				m_BufferBase += Size;
				return ReadedBytes;
			}

			// 读取的大小小于缓存容量. 我们尽量地完全填充缓存 
			VASSERT(m_CurPos==m_BufferBase+m_BufferCount);
			m_BufferBase = m_CurPos;
			m_BufferCount = VMin<UINT>(BUFFER_SIZE, m_Size-m_CurPos);
			m_BufferCount = m_File->Read(m_Buffer, m_BufferCount);
			
			// 拷贝到目标内存
			Copy = VMin( Size, m_BufferBase+m_BufferCount-m_CurPos );
			if( Copy<=0 )
			{
				return ReadedBytes;
			}
		}
		VMemcpy(Dest, m_Buffer+m_CurPos-m_BufferBase, Copy);
		ReadedBytes += Copy;
		m_CurPos   += Copy;
		Size   -= Copy;
		Dest  = (BYTE*)Dest + Copy;
	}
	return ReadedBytes;
}

UINT VReadFileStream::Write(const void* Dest, UINT Size)
{
	VASSERT(!"Readonly");
	return 0;
}

UINT VReadFileStream::Tell() const 
{ 
	return m_CurPos; 
}

void VReadFileStream::Flush()
{

}

//////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////
VWriteFileStream::VWriteFileStream()
	:m_File(NULL),m_CurPos(0),m_BufferCount(0)
{
}
VWriteFileStream::VWriteFileStream(VFile* File)
	:m_File(File),m_CurPos(0),m_BufferCount(0)
{
	
}

VWriteFileStream::~VWriteFileStream()
{
	Close();
}

BOOL VWriteFileStream::Open(const char* FileName)
{
	VASSERT(VFileSystem);
	Close();
	VFile* File = VFileSystem->OpenFile(FileName, VFOM_WRITE);
	if (File)
	{
		m_File = File;
		return TRUE;
	}
	return FALSE;
}

void VWriteFileStream::Close() 
{
	if (m_File)
	{
		Flush();
		m_File->Close(); 
		m_File = NULL;
	}
	m_CurPos = 0;
	m_BufferCount = 0;
}

UINT VWriteFileStream::Size() 
{ 
	VASSERT(m_File);
	return m_CurPos;
}

BOOL VWriteFileStream::Seek(UINT Pos)
{
	VASSERT(Pos>=0);
	VASSERT(m_File);

	Flush();

	if(!m_File->Seek(Pos))
	{
		m_CurPos = m_File->Tell();
		return FALSE;
	}
	m_CurPos       = Pos;
	return TRUE;
}

UINT VWriteFileStream::Read(void* Dest, UINT Size)
{
	VASSERT(m_File);
	VASSERT(!"WriteOnly");
	return 0;
}

UINT VWriteFileStream::Write(const void* Dest, UINT Size)
{
	m_CurPos += Size;
	UINT Copy;
	UINT WritedBytes = 0;
	while( Size > (Copy= BUFFER_SIZE-m_BufferCount) )
	{
		VMemcpy(m_Buffer+m_BufferCount, Dest, Copy );
		m_BufferCount += Copy;
		Size      -= Copy;
		Dest       = (BYTE*)Dest + Copy;
		WritedBytes += Copy;
		Flush();
	}
	if( Size )
	{
		VMemcpy( m_Buffer+m_BufferCount, Dest, Size );
		WritedBytes += Size;
		m_BufferCount += Size;
	}
	return WritedBytes;
}

UINT VWriteFileStream::Tell() const 
{ 
	return m_CurPos; 
}

void VWriteFileStream::Flush()
{
	VASSERT(m_File);
	if( m_BufferCount )
	{
		INT Result=0;
		UINT WritedBytes = m_File->Write(m_Buffer, m_BufferCount);
		VASSERT(WritedBytes == m_BufferCount);
		m_BufferCount = 0;
	}
}