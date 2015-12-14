#pragma once

class VCoreApi VScene : public cocos2d::Scene , public VAllocator
{
public:
	VScene();
	~VScene();
	virtual void DeleteThis();
	virtual void cleanup();

	void HideRootExcept(cocos2d::Node* Child);
	void RestoreHidedRoots();
protected:
	cocos2d::Layer* mMainLayer;		
	cocos2d::Layer* mPopupLayer;
	VPODArray<cocos2d::Node*> mHidedRoots;
};
