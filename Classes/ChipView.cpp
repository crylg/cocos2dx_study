#include "stdafx.h"
#include "ChipView.h"



ChipView::ChipView()
{
}


ChipView::~ChipView()
{
}

bool ChipView::init()
{
	this->initRootViewAndrootOfRequestNode();
	this->requestChildTag();

	return true;
}

void ChipView::initRootViewAndrootOfRequestNode()
{
	_rootView = this->LoadView("game/play_menu.csb");
	this->addChild(_rootView);
	this->setPosition(805, 0);
	_rootOfRequestNode = _rootView->getChildByTag(eTag::head);
}

void ChipView::requestChildTag()
{
	_discardBtn = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(eTag::discard);
	_followBtn = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(eTag::follow);
	_oneBtn = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(eTag::one);
	_fiveBtn = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(eTag::discard);
	_allInBtn = (cocos2d::ui::Button*)_rootOfRequestNode->getChildByTag(eTag::discard);
}
