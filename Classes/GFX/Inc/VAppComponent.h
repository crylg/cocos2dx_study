#pragma once
#include "VComponent.h"

/**
 * AppComponent 是属于应用程序的组件, 应用程序组件会有相互依赖关系 
 */
class VAppComponent : public VComponent
{
public:
	VAppComponent();
	~VAppComponent();

	inline const VName& GetName() const { return mName; }

protected:
	VName mName;		// 组件名称
};

