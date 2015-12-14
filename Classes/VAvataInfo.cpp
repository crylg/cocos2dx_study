#include "stdafx.h"
#include "VAvataInfo.h"

USING_NS_CC;


//void VAvataInfo::initLayout(cocos2d::Node* parent)
//{
//	parnt->addChild(_root);
//}

//void VAvataInfo::initData(const std::string name, std::string money, std::string avata_flag)
void VAvataInfo::initData(void* data)
{
	_nameText->setString("zaodian");
	_moneyText->setString("1111112");
}

void VAvataInfo::resetData()
{

}

bool VAvataInfo::init()
{
	this->initRootViewAndrootOfRequestNode();
	this->requestChildTag();
	return true;
}

void VAvataInfo::requestChildTag()
{
	assert(_rootView);
	assert(_rootOfRequestNode);
	if (_rootView==nullptr) return;
	if (_rootOfRequestNode == nullptr) return;
	_avataSprite = (Sprite*)_rootOfRequestNode->getChildByTag(eTag::avata_sprite);
	_nameText = (cocos2d::ui::Text*)_rootOfRequestNode->getChildByTag(eTag::name_text);
	_moneyText = (cocos2d::ui::Text*)_rootOfRequestNode->getChildByTag(eTag::money_text);
}

void VAvataInfo::initRootViewAndrootOfRequestNode()
{
	
}


