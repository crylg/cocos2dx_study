#include "stdafx.h"
#include "PlayerInfoView.h"
#include "VAvataInfo.h"

USING_NS_CC;
bool PlayerInfoView::init()
{
	createBg();
	createAvataInfo();
	return true;
}

PlayerInfoView* PlayerInfoView::create()
{
	PlayerInfoView *piv = new (std::nothrow) PlayerInfoView();
	if (piv && piv->init())
	{
		return piv;
	}
	CC_SAFE_DELETE(piv);
	return nullptr;
}


void PlayerInfoView::createBg()
{
	_vBg = this->LoadView("game/player_time.csb");
	addChild(_vBg);
}

void PlayerInfoView::createAvataInfo()
{
	_vAvataInfo = VAvataInfo::create();
	this->addChild(_vAvataInfo);
}

