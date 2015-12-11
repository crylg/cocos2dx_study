#include "VStdAfx.h"
#include "VDeviceManager.h"
#include "CCFileUtils.h"

#if VPLATFORM_ANDROID
#if !defined(MAX_PATH)
#define MAX_PATH  256
#endif

#include "../platform/android/jni/JniHelper.h"

static cocos2d::JniMethodInfo j_GetDeviceGuid;

static cocos2d::JniMethodInfo j_GetDeviceModel;

int Android_GetDeviceGuid(char* DeviceGuid, int MaxLen)
{
	VASSERT(MaxLen >= 64);
	
	if (j_GetDeviceGuid.env == nullptr)
	{
		if(!cocos2d::JniHelper::getStaticMethodInfo(j_GetDeviceGuid, "com/zd/utils/Device", "GetDeviceGuid", "()Ljava/lang/String;"))
		{
			return -1;
		}
	}
	jstring jDeviceGuid = (jstring)j_GetDeviceGuid.env->CallStaticObjectMethod(j_GetDeviceGuid.classID, j_GetDeviceGuid.methodID);
	std::string sDeviceGuid = cocos2d::JniHelper::jstring2string(jDeviceGuid);
	strncpy(DeviceGuid, sDeviceGuid.c_str(), MaxLen);
	return sDeviceGuid.length();
}



int Android_GetDeviceModel(char* DeviceModel, int MaxLen)
{
	VASSERT(MaxLen >= 64);

	if (j_GetDeviceModel.env == nullptr)
	{
		if (!cocos2d::JniHelper::getStaticMethodInfo(j_GetDeviceModel, "com/zd/utils/Device", "GetDeviceModel", "()Ljava/lang/String;"))
		{
			return -1;
		}
	}
	jstring jDeviceModel = (jstring)j_GetDeviceModel.env->CallStaticObjectMethod(j_GetDeviceModel.classID, j_GetDeviceModel.methodID);
	std::string sDeviceModel = cocos2d::JniHelper::jstring2string(jDeviceModel);
	strncpy(DeviceModel, sDeviceModel.c_str(), MaxLen);
	return sDeviceModel.length();
}
#else 

#include <Commdlg.h>
#endif 


VDeviceManager::VDeviceManager()
{
}


VDeviceManager::~VDeviceManager()
{
}

const char* VDeviceManager::GetIMEI()
{
	static char sIMEI[128];
	if (sIMEI[0] == 0)
	{
#if VPLATFORM_WIN32
		VStrcpy(sIMEI, 128, "Windows Dummy IMEI");
#elif VPLATFORM_ANDROID
		
		int Ret = Android_GetDeviceGuid(sIMEI, 128);
		cocos2d::log("Android_GetDeviceGuid(sIMEI, 128)","");
		if (Ret == -1)
		{
			// failed . prevent next query.
			strcpy(sIMEI, "Invalid Android IMEI");
		}

#elif VPLATFORM_IOS
		VStrcpy(sIMEI, 128, "IOS Dummy IMEI");
#else 
		VStrcpy(sIMEI, 128, "IMEI");
#endif 
	}
	return sIMEI;
}

const char* VDeviceManager::GetPlatform()
{
	static char sPlatform[128];
	if (sPlatform[0] == 0)
	{
#if VPLATFORM_WIN32
		VStrcpy(sPlatform, 128, "Windows Emulator");
#elif VPLATFORM_ANDROID
		VStrcpy(sPlatform, 128, "Android 4.02");
#elif VPLATFORM_IOS
		VStrcpy(sPlatform, 128, "IOS 7.0");
#else 
		VStrcpy(sPlatform, 128, "Unknown Platform");
#endif 
	}
	return sPlatform;
}
//获取设备名
const char* VDeviceManager::GetDeviceName()
{
	static char sNAME[128];
	if (sNAME[0] == 0)
	{
#if VPLATFORM_WIN32
		VStrcpy(sNAME, 128, "IPHONE");
#elif VPLATFORM_ANDROID

		int Ret = Android_GetDeviceModel(sNAME, 128);
		cocos2d::log("Android_GetDeviceModel(sNAME, 128)", "");
		if (Ret == -1)
		{
			// failed . prevent next query.
			strcpy(sNAME, "Invalid Android MODEL");
		}

#elif VPLATFORM_IOS
		VStrcpy(sNAME, 128, "IOS Dummy MODEL");
#else 
		VStrcpy(sNAME, 128, "MODEL");
#endif 
	}
	return sNAME;
}

const char* VDeviceManager::GetUserChannel()
{
	static char Channel[MAX_PATH] = { 0 };
	if (Channel[0] == 0)
	{
#if VPLATFORM_WIN32
		strcpy(Channel, "internal_dev");
#elif VPLATFORM_ANDROID

		{
			cocos2d::JniMethodInfo jMethod;
			if (cocos2d::JniHelper::getStaticMethodInfo(jMethod, "com/zd/utils/Device", "GetChannel", "()Ljava/lang/String;"))
			{
				jstring jstr = (jstring)jMethod.env->CallStaticObjectMethod(jMethod.classID, jMethod.methodID);
				std::string str = cocos2d::JniHelper::jstring2string(jstr);
				strncpy(Channel, str.c_str(), MAX_PATH);
			}
			else
			{
				strcpy(Channel, "android_unk_channel");
			}

		}

#endif
	}
	return Channel;
}

#pragma region "Image Selection"
#include "VImageHelper.h"
static VDeviceManager::ImageSelectedCallback sImageSelectedCallback = nullptr;
static void* sImageSelectedUserData = nullptr;


#if VPLATFORM_WIN32


#elif VPLATFORM_ANDROID

#endif 



BOOL VDeviceManager::SelectImage(ImageSelectedCallback SelectedCallback, void* UserData)
{
	//InitCommonControls
	if (!SelectedCallback )
	{
		return FALSE;
	}

	BOOL Ret = FALSE;
	sImageSelectedCallback = SelectedCallback;
	sImageSelectedUserData = UserData;
#if VPLATFORM_WIN32
	{
		BOOL bHandled = FALSE;
		OPENFILENAMEA ofn = { 0 };
		CHAR szSelectedFile[MAX_PATH] = { 0 };
		ofn.lStructSize = sizeof(OPENFILENAMEA);
		ofn.hwndOwner = NULL;
		ofn.hInstance = NULL;
		ofn.lpstrFilter = "All image files\0*.png;*.jpg\0\0";
		ofn.lpstrTitle = "select your picture";
		ofn.lpstrFile = szSelectedFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST;
		do
		{
			if (GetOpenFileNameA(&ofn))
			{
				int width, height;
				if (VImageHelper::GetImageDimension(szSelectedFile, width, height) && width <= 512 && height <= 512)
				{
					SelectedImage result;
					result.path = szSelectedFile;
					bHandled = sImageSelectedCallback(result, sImageSelectedUserData);
					Ret = TRUE;
				}
				else
				{
					MessageBoxA(ofn.hwndOwner, "Please select size smaller than 512x512", "Set Picture Failed", MB_OK | MB_ICONERROR);
				}
			}
			else
			{
				break;
			}
		} while (!bHandled);
	}

#elif VPLATFORM_ANDROID

	{
	}

#endif 


	return Ret;
}

#pragma endregion "Image Selection"


const char* VDeviceManager::GetCacheDirection()
{
	static  char CacheDirection[MAX_PATH] = {0};
	if (CacheDirection[0] == 0)
	{
#if VPLATFORM_WIN32

#elif VPLATFORM_ANDROID

#endif 

	}
	return CacheDirection;
}

const char* VDeviceManager::GetAppCacheDirection()
{
	static char AppCacheDirection[MAX_PATH] = { 0 };
	if (AppCacheDirection[0] == 0)
	{
#if VPLATFORM_WIN32
		const std::string CachePath =  cocos2d::FileUtils::getInstance()->getWritablePath();
		if (!CachePath.empty())
		{
			strcpy(AppCacheDirection, CachePath.c_str());
		}
#elif VPLATFORM_ANDROID

		{
			cocos2d::JniMethodInfo jMethod;
			if (cocos2d::JniHelper::getStaticMethodInfo(jMethod, "com/zd/utils/Device", "GetSharedCacheDir", "()Ljava/lang/String;"))
			{
				jstring jCacheDir = (jstring)jMethod.env->CallStaticObjectMethod(jMethod.classID, jMethod.methodID);
				std::string sCacheDir = cocos2d::JniHelper::jstring2string(jCacheDir);
				strncpy(AppCacheDirection, sCacheDir.c_str(), MAX_PATH);
			}
			else
			{
				strcpy(AppCacheDirection, "ZDAppCache");
			}
			
		}

#endif
	}
	return AppCacheDirection;
}

void VDeviceManager::setScreenBright(bool bBright)
{
#if VPLATFORM_WIN32

#elif VPLATFORM_ANDROID
	cocos2d::JniMethodInfo jMethod;
	if (cocos2d::JniHelper::getStaticMethodInfo(jMethod, "com/zd/utils/Device", "JavaSetScreenBright", "(B)V"))
	{
		jMethod.env->CallStaticObjectMethod(jMethod.classID, jMethod.methodID, bBright);
	}


#elif VPLATFORM_IOS

#endif

}

#if 0 
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#if VPLATFORM_ANDROID
extern "C"
{
	/**
	* this method is called by java code to init width, height and pixels data
	*/
	JNIEXPORT void JNICALL Java_com_zd_utils_AppPatchManager_OnNewVersion(JNIEnv*  env, jobject thiz, int Version, jstring VersionName)
	{
		
	}
};

#endif 


BOOL VDeviceManager::AppPatch()
{
#if VPLATFORM_WIN32
	return TRUE;
#elif VPLATFORM_ANDROID
	cocos2d::JniMethodInfo jMethod;
	if (cocos2d::JniHelper::getStaticMethodInfo(jMethod, "com/zd/utils/AppPatchManager", "BeginPatch", "()V"))
	{
		jMethod.env->CallStaticObjectMethod(jMethod.classID, jMethod.methodID);
	}


#elif VPLATFORM_IOS

#endif
	return TRUE;
}

#endif 






void VDeviceManager::Shake(int MilliSeconds)
{
#if VPLATFORM_ANDROID
	static cocos2d::JniMethodInfo j_Shake;
	if (j_Shake.env == nullptr)
	{
		if (!cocos2d::JniHelper::getStaticMethodInfo(j_Shake, "com/zd/utils/Device", "Shake", "(I)V"))
		{
			j_Shake.env = (JNIEnv *)1;
			return;
		}
	}else if (j_Shake.env == (JNIEnv *)1)
	{
		return;
	}
	jint ms = MilliSeconds;
	j_Shake.env->CallStaticObjectMethod(j_Shake.classID, j_Shake.methodID, ms);
#else 
#endif 
}

static int cur_provides = 0;
VDeviceManager::PROVIDERS VDeviceManager::GetProvides()
{
	return (PROVIDERS)cur_provides;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
	void Java_com_zd_utils_Device_setProvidersName(JNIEnv* env, jobject thiz,jint p)
	{
		//p返回值为0,46000,46001,46003
		cur_provides = p;
		VLOG("Java_com_zd_utils_Device_setProvidersName C++%d", VDeviceManager::GetProvides());
		
	}
}
#endif