#pragma once
#include "VComponent.h"

/**
 * AppComponent ������Ӧ�ó�������, Ӧ�ó�����������໥������ϵ 
 */
class VAppComponent : public VComponent
{
public:
	VAppComponent();
	~VAppComponent();

	inline const VName& GetName() const { return mName; }

protected:
	VName mName;		// �������
};

