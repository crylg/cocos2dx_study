#pragma once
#include "cocos2d.h"

class VAvataInfo;//�Աߵ���Ϣ
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
	//�Ƿ�Ϊׯ��
	bool _juddgeIsFarmer;
	//VAvata* _vAvata;
	VAvataInfo* _vAvataInfo;
	cocos2d::Node* _vBg;//����
	void createBg();
	void createAvataInfo();
	//void createAvata();
};
//playerInfoView implements INode
//playerInfoView->getSkin return INode
//addChild(INode)
//parent.addChild(_playerInfoView->getSkin());//ʹ���м��س�Ϊ�������,û��Ҫ,��������Ҳ�ð�INode����ȫ��д 

