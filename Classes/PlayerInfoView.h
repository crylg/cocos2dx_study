#pragma once
#include "cocos2d.h"

class VAvataInfo;//旁边的信息
class PlayerInfoView :public VView
{

public:
	static PlayerInfoView* create();


	enum ePlayerType
	{
		Self,
		Other
	};

	bool init();

private:
	//是否为庄家
	bool _juddgeIsFarmer;
	//VAvata* _vAvata;
	VAvataInfo* _vAvataInfo;
	cocos2d::Node* _vBg;//背景
	void createBg();
	void createAvataInfo();
	//void createAvata();
};
//playerInfoView implements INode
//playerInfoView->getSkin return INode
//addChild(INode)
//parent.addChild(_playerInfoView->getSkin());//使所有加载成为代理加载,没必要,就算重载也得把INode属性全重写 

