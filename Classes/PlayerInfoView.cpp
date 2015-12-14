#include "stdafx.h"
#include "PlayerInfoView.h"
#include "VAvataInfo.h"
#include "VAvataInfoOther.h"
#include "VAvataInfoSelf.h"
USING_NS_CC;
bool PlayerInfoView::init()
{
	return true;
}

void PlayerInfoView::layout(ePlayerType type, ePosition p)
{
	createBg(type);
	createAvataInfo(type);
	_vBg->setPosition(this->getPositionByEPosition(p));
}


void PlayerInfoView::createBg(ePlayerType type)
{
	
	std::string str= "game/";
	switch (type)
	{
	case ePlayerType::Other:
		str+= "player_time.csb";
		break;
	case ePlayerType::Self:
		str+= "user_time.csb";
		break;
	default:
		break;
	}
	_vBg = this->LoadView(str.c_str());
	addChild(_vBg);
}

void PlayerInfoView::createAvataInfo(ePlayerType type)
{
	_vAvataInfo = VAvataInfoOther::create();
	switch (type)
	{
	case ePlayerType::Self:
		_vAvataInfo = VAvataInfoSelf::create();
		break;
	case ePlayerType::Other:
		_vAvataInfo = VAvataInfoOther::create();
		break;
	default:
		break;
	}
	
	_vBg->addChild(_vAvataInfo);
}

cocos2d::Vec2 PlayerInfoView::getPositionByEPosition(ePosition p)
{
	Vec2 tmpP;
	switch (p)
	{
		case ePosition::LB:
			tmpP.setPoint(71,273);
			break;
		case ePosition::LT:
			tmpP.setPoint(183.5,493);
			break;
		case ePosition::RB:
			tmpP.setPoint(1215,273);
			break;
		case ePosition::RT:
			tmpP.setPoint(1103,493);
			break;
		default:
			tmpP.setZero();
			break;
	}
	return tmpP;
}

