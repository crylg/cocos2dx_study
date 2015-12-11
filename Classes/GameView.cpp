#include "stdafx.h"
#include "GameView.h"

#include "cocostudio/CocoStudio.h"
#include "PlayerInfoView.h"
USING_NS_CC;


Node* LoadView(const char* FileName)
{
	VASSERT(FileName && FileName[0]);

	Node* RootNode = CSLoader::createNode(FileName);
	return RootNode;
}


#define LOAD_VIEW(Name)  LoadView(Name)

/************************************************************************************/
GameView::GameView()
{
}


GameView::~GameView()
{
	delete _playerInfoView;
}

Scene* GameView::createScene()
{
	auto scene = Scene::create();
	auto layer = GameView::create();
	scene->addChild(layer);
	return scene;
}

bool GameView::init()
{
	if (!Layer::init())
	{
		return false;
	}

 	Node* node =LoadView("game/game.csb");
	if (!node) return NULL;
	addChild(node);
	_playerInfoView = PlayerInfoView::create();
	addChild(_playerInfoView);
	CCLOG("x:%i,y:%i", _playerInfoView->getPosition().x, _playerInfoView->getPosition().y);
	_playerInfoView->setPosition(100, 0);

	return true;
}
