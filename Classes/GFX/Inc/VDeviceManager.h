#pragma once


class VCoreApi VDeviceManager
{
public:
	VDeviceManager();
	~VDeviceManager();
	static const char* GetIMEI();

	static const char* GetDeviceName();

	static const char* GetPlatform();

	// 获得用户渠道
	static const char* GetUserChannel();

	/**
	 * 获得应用程序共享的缓存目录 
	 */
	static const char* GetCacheDirection();

	static const char* GetAppCacheDirection();


	/**
	 * 从设备中选择的照片数据 
	 */
	struct SelectedImage
	{
		std::string path;
		void*		Data = nullptr;
		size_t		DataSize = 0;
	};

	/**
	 * 选择照片的回调函数. 
	 * @param Image 选择的照片信息
	 * @param UserData 传入的用户数据
	 * 
	 */
	typedef BOOL (*ImageSelectedCallback)(const SelectedImage& Image, void* UserData);


	/**
	 * 从设备中选择/或者拍摄一张照片.  
	 */
	static BOOL SelectImage(ImageSelectedCallback SelectedCallback, void* UserData);


	// 设置手机屏幕是否常亮
	static void setScreenBright(bool bBright);

	// 震动设备
	static void Shake(int MilliSeconds = 800);

	enum PROVIDERS
	{
		UNKNOWN = 0, //未知运营商
		CUCC = 46001,//联通
		CMCC = 46000,//移动
		CTCC = 46003 //电信
	};

	static PROVIDERS GetProvides();

};

