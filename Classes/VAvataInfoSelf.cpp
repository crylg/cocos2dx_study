#include "stdafx.h"
#include "VAvataInfoSelf.h"

USING_NS_CC;

VAvataInfoSelf::VAvataInfoSelf()
{
}


VAvataInfoSelf::~VAvataInfoSelf()
{
}

void VAvataInfoSelf::initData(void* data)
{

}

void VAvataInfoSelf::resetData()
{

}

void VAvataInfoSelf::requestChildTag()
{
	VAvataInfo::requestChildTag();
	_farmerSprite = (Sprite*)_rootOfRequestNode->getChildByTag(VAvataInfoSelf::eTag::farmer_sprite);
	_moneyButton = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(VAvataInfoSelf::eTag::money_button);
	_chatButton = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(VAvataInfoSelf::eTag::chat_button);
}

void VAvataInfoSelf::initRootViewAndrootOfRequestNode()
{
	VAvataInfo::initRootViewAndrootOfRequestNode();
	_rootView = this->LoadView("game/user.csb");
	this->addChild(_rootView);
	_rootOfRequestNode = _rootView;
}
