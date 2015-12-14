#include "stdafx.h"
#include "VAvataInfoOther.h"
#include "cocos2d.h"
USING_NS_CC;
VAvataInfoOther::VAvataInfoOther()
{
}


VAvataInfoOther::~VAvataInfoOther()
{
}

void VAvataInfoOther::initData(void* data)
{
	VAvataInfo::initData(data);
}

void VAvataInfoOther::resetData()
{

}

void VAvataInfoOther::requestChildTag()
{
	VAvataInfo::requestChildTag();
	_farmerSpriteLeft = (Sprite*)_rootOfRequestNode->getChildByTag(VAvataInfoOther::eTag::farmer_left_sprite);
	_farmerSpriteRight = (Sprite*)_rootOfRequestNode->getChildByTag(VAvataInfoOther::eTag::farmer_right_sprite);
	_avataNullSprite = (Sprite*)_rootOfRequestNode->getChildByTag(VAvataInfoOther::eTag::avata_null_sprite);
}

void VAvataInfoOther::initRootViewAndrootOfRequestNode()
{
	_rootView = this->LoadView("game/player.csb");
	this->addChild(_rootView);
	_rootOfRequestNode = _rootView->getChildByTag(VAvataInfo::eTag::head);
}
