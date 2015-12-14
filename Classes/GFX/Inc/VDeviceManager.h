#pragma once


class VCoreApi VDeviceManager
{
public:
	VDeviceManager();
	~VDeviceManager();
	static const char* GetIMEI();

	static const char* GetDeviceName();

	static const char* GetPlatform();

	// ����û�����
	static const char* GetUserChannel();

	/**
	 * ���Ӧ�ó�����Ļ���Ŀ¼ 
	 */
	static const char* GetCacheDirection();

	static const char* GetAppCacheDirection();


	/**
	 * ���豸��ѡ�����Ƭ���� 
	 */
	struct SelectedImage
	{
		std::string path;
		void*		Data = nullptr;
		size_t		DataSize = 0;
	};

	/**
	 * ѡ����Ƭ�Ļص�����. 
	 * @param Image ѡ�����Ƭ��Ϣ
	 * @param UserData ������û�����
	 * 
	 */
	typedef BOOL (*ImageSelectedCallback)(const SelectedImage& Image, void* UserData);


	/**
	 * ���豸��ѡ��/��������һ����Ƭ.  
	 */
	static BOOL SelectImage(ImageSelectedCallback SelectedCallback, void* UserData);


	// �����ֻ���Ļ�Ƿ���
	static void setScreenBright(bool bBright);

	// ���豸
	static void Shake(int MilliSeconds = 800);

	enum PROVIDERS
	{
		UNKNOWN = 0, //δ֪��Ӫ��
		CUCC = 46001,//��ͨ
		CMCC = 46000,//�ƶ�
		CTCC = 46003 //����
	};

	static PROVIDERS GetProvides();

};

