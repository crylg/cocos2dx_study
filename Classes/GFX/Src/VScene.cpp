#include "VStdAfx.h"
#include "VScene.h"
#include "VGame.h"
USING_NS_CC;

VScene::VScene()
{

	init();
	autorelease();
}

VScene::~VScene()
{
}

void VScene::DeleteThis()
{
	VDelete this;
}

void VScene::cleanup()
{
	cocos2d::Application* App = cocos2d::Application::getInstance();
	VGame* GameApp = (VGame*)App;
	if (GameApp)
	{
		GameApp->ShutdownGame();
	}
}

void VScene::HideRootExcept(cocos2d::Node* Child)
{
	RestoreHidedRoots();
	int HideZOrder = Child->getLocalZOrder();
	for (int Index = 1; Index < (int)_children.size(); ++Index)
	{
		cocos2d::Node* c = _children.at(Index);
		int zOrder = c->getLocalZOrder();
		if (zOrder > HideZOrder)
		{
			continue;
		}
		if (c == Child)
		{
			return;
		}
		if (c->isVisible())
		{
			c->setVisible(false);
			c->retain();				// HACK. cocos2d::Node 并没有提供一个OnChildRemoved的借口便于我们监听节点移除事件, 虽然我们能重载removeChild 等, 但是鉴于接口过多, 不采用这种方式. 
			// 我们通过增加引用计数的方式来避免隐藏节点被删除的问题. 
			mHidedRoots.PushBack(c);
		}
	}
	
}

void VScene::RestoreHidedRoots()
{
	for (int Index = 0; Index < (int)mHidedRoots.Size(); ++Index)
	{
		cocos2d::Node* Child = mHidedRoots[Index];
		Child->setVisible(true);
		Child->release();
	}
	mHidedRoots.Clear();
}