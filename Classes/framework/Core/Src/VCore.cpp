#include "VStdAfx.h"
#include "VCore.h"
#include "KMemory.h"
#include "KFileSystem.h"
#include "VInterpolate.h"
#include "KTaskManager.h"

const VGuid VGUID_NULL(0,0);

VStackMemoryPool VMainStackPool(64* 1024);

extern void KStringTableInit();
extern void KStringTableShutdown();

BOOL VCoreInit()
{
	float Value = VEaseIn<VEasingCubic>(1.0f, 2.0f, 0.1f);

	if(!VPlatformInit())
	{
		return FALSE;
	}

	if (!KInitMemory())
	{
		return FALSE;
	}
#if VPLATFORM_WIN32
	if(!KFileSystemInit())
	{
		return FALSE;
	}

	KStringTableInit();



	// 初始化任务管理器
	vTaskManager = new KTaskManager;
	vTaskManager->Init(4);
#endif 
	return TRUE;
}

void VCoreShutdown()
{
#if VPLATFORM_WIN32
	vTaskManager->Shutdown();
	delete vTaskManager;
	vTaskManager = NULL;

	KStringTableShutdown();
#endif 
	KFileSystemShutdown();
	KShutdownMemory();
}

VGuid VGenGuid()
{
	VSystemTime SysTime;
	VGetSystemTime(SysTime);
	VGuid   GUID;
	GUID.A = SysTime.Day;		// 5
	GUID.A |= (((UINT)SysTime.Hour) << 16);	//5

	GUID.B = SysTime.Month; //4
	GUID.B |= (((UINT)SysTime.MSec) << 16); //16

	UINT C,D;
	C = SysTime.Sec; //6
	C |= (((UINT)SysTime.Min) << 16); //6

	D = SysTime.Year ^ (UINT)VGetCycles();

	GUID.A |= C;
	GUID.B |= D;

	return GUID;
}

VGuid VGenGuid(const char* Name)
{
	return VGuid();
}


UINT VCrc32(const void* InData, UINT Length, UINT CRC )
{
	BYTE* Data = (BYTE*)InData;
	CRC = ~CRC;
	for( UINT i=0; i<Length; i++ )
		CRC = (CRC << 8) ^ vc_CRCTable[(CRC >> 24) ^ Data[i]];
	return ~CRC;
}
//
//UINT VStrCrc(const char* Data )
//{
//	VASSERT(Data);
//	INT Length = strlen( Data );
//	UINT CRC = 0xFFFFFFFF;
//	for( INT i=0; i<Length; i++ )
//	{
//		TCHAR C   = Data[i];
//		INT   CL  = (C&255);
//		CRC       = (CRC << 8) ^ vc_CRCTable[(CRC >> 24) ^ CL];;
//#if UNICODE
//		INT   CH  = (C>>8)&255;
//		CRC       = (CRC << 8) ^ vc_CRCTable[(CRC >> 24) ^ CH];;
//#endif
//	}
//	return ~CRC;
//}

UINT VStrCrc(const char* Str, UINT CRC )
{
	VASSERT(Str);
	INT Length = strlen( Str );
	CRC = ~CRC;
	for( INT i=0; i<Length; i++ )
	{
		char C   = Str[i];
		INT   CL  = (C&255);
		CRC       = (CRC << 8) ^ vc_CRCTable[(CRC >> 24) ^ CL];;
#if UNICODE
		INT   CH  = (C>>8)&255;
		CRC       = (CRC << 8) ^ vc_CRCTable[(CRC >> 24) ^ CH];;
#endif
	}
	return ~CRC;
}

UINT LStriCrc( const char* Data )
{
	/*INT Length = strlen( Data );
	UINT CRC = 0xFFFFFFFF;
	for( INT i=0; i< Length; i++ )
	{
	TCHAR C   = toupper(Data[i]);
	INT   CL  = (C&255);
	CRC       = (CRC << 8) ^ LetheCRCTable[(CRC >> 24) ^ CL];
	#if UNICODE
	INT   CH  = (C>>8)&255;
	CRC       = (CRC << 8) ^ LetheCRCTable[(CRC >> 24) ^ CH];
	#endif
	}
	return ~CRC;*/
	return 0;
}


const char* VAppPath()
{
	return NULL;
}

const char* VDataPath()
{
	return NULL;
}