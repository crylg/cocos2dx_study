#include "stdafx.h"
#include "GameView.h"

#include "cocostudio/CocoStudio.h"
#include "PlayerInfoView.h"
#include "ChipView.h"
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
	/*delete _playerInfoView;*/
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
	/*_playerInfoView = PlayerInfoView::create();
	if (_playerInfoView == nullptr)
	{
		assert(_playerInfoView);
		return false;
	}
	_playerInfoView->retain();
	_playerInfoView->layout(PlayerInfoView::Other, PlayerInfoView::RT);
	addChild(_playerInfoView);
	CCLOG("x:%i,y:%i", _playerInfoView->getPosition().x, _playerInfoView->getPosition().y);
	_playerInfoView->setPosition(100, 0);
*/
	//初始化其它四人客户端
	std::vector<PlayerInfoView*> pivs;
	int i;
	for (i = 0; i < 4; i++)
	{
		PlayerInfoView* piv = PlayerInfoView::create();
		if (piv == nullptr)
		{
			assert(piv);
			return false;
		}
		piv->layout(PlayerInfoView::Other, PlayerInfoView::ePosition(i));
		addChild(piv);
		pivs.push_back(piv);
	}
	//初始化本地
	PlayerInfoView* self_piv = PlayerInfoView::create();
	if (self_piv == nullptr)
	{
		assert(self_piv);
		return false;
	}
	self_piv->layout(PlayerInfoView::Self, PlayerInfoView::ePosition::P_Self);
	addChild(self_piv);

	//初始化底部投注区
	ChipView* cv = ChipView::create();
	if (cv == nullptr)
	{
		assert(cv);
		return false;
	}
	addChild(cv);
	return true;
}
