#pragma once
#include "cocostudio/CocosStudioExport.h"
#include "cocostudio/ActionTimeline/CCActionTimeline.h"

enum LayoutType
{
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	CENTER,
};

class VCoreApi VView : public cocos2d::Node, public VAllocator
{
public:
	VView();
	virtual ~VView();

	virtual void DeleteThis();
	BOOL LoadViewFrom(const char* FileName);
	static cocos2d::Node* LoadView(const char* FileName);
	static cocostudio::timeline::ActionTimeline* LoadAction(const char* FileName);

	

protected:
	virtual BOOL OnLoadView(cocos2d::Node* Root);
	void DoLayout(Node* node, LayoutType type = CENTER);

protected:
	
};

