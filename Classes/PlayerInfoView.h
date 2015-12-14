#pragma once
#include "cocos2d.h"

class VAvataInfo;//�Աߵ���Ϣ
class PlayerInfoView :public VView
{

public:
	//static PlayerInfoView* create();
	CREATE_FUNC(PlayerInfoView);

	enum ePlayerType
	{
		Self,
		Other
	};
	enum ePosition
	{
		RB,
		RT,
		LT,
		LB,
		P_Self
	};
	bool init();
	void layout(ePlayerType type, ePosition p);
private:
	//�Ƿ�Ϊׯ��
	bool _juddgeIsFarmer;
	//VAvata* _vAvata;
	VAvataInfo* _vAvataInfo;
	cocos2d::Node* _vBg;//����
	void createBg(ePlayerType type);
	void createAvataInfo(ePlayerType type);
	cocos2d::Vec2 getPositionByEPosition(ePosition p);
	//void createAvata();
	
};
//playerInfoView implements INode
//playerInfoView->getSkin return INode
//addChild(INode)
//parent.addChild(_playerInfoView->getSkin());//ʹ���м��س�Ϊ�������,û��Ҫ,��������Ҳ�ð�INode����ȫ��д 

