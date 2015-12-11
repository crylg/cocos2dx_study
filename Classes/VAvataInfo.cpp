#include "stdafx.h"
#include "VAvataInfo.h"

USING_NS_CC;
enum eTag
{
avata_sprite=76,
farmer_left_sprite=1394,
farmer_right_sprite= 82,
name_text= 79,
money_text= 78

};

VAvataInfo* VAvataInfo::create()
{
	VAvataInfo *vai = new (std::nothrow) VAvataInfo();
	if (vai && vai->init())
	{
		vai->autorelease();
		return vai;
	}
	CC_SAFE_DELETE(vai);
	return nullptr;
}

bool VAvataInfo::init()
{
	Node* root= this->LoadView("game/player.csb");
	addChild(root);
	_avataSprite=(Sprite*)root->getChildByTag(eTag::avata_sprite);
	_nameText = (cocos2d::ui::Text*)root->getChildByTag(eTag::name_text);
	_moneyText = (cocos2d::ui::Text*)root->getChildByTag(eTag::money_text);
	_farmerSpriteLeft = (Sprite*)root->getChildByTag(eTag::farmer_left_sprite);
	_farmerSpriteRight = (Sprite*)root->getChildByTag(eTag::farmer_right_sprite);
	
	_nameText->setString("zaodian");
	_moneyText->setString("1111112");
	_farmerSpriteRight->removeFromParent();
	//....to do
	return true;
}
