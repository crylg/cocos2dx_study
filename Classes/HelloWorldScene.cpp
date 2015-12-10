#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::setViewpointCenter(Vec2 position)
{
	
	int x = MAX(position.x, _screenWidth);
	int y = MAX(position.y, _screenHeight);

	x = MIN(x,(_tileMap->getMapSize().width*_tileMap->getTileSize().width-_screenWidth/2));
	y = MIN(y, (_tileMap->getMapSize().height*_tileMap->getTileSize().height - _screenHeight / 2));
	Point actualPosition = Vec2(_screenWidth / 2, _screenHeight / 2);

	Point centerOfView = Vec2(_screenWidth / 2, _screenHeight / 2);

	Point viewPoint = centerOfView - actualPosition;

	this->setPosition(viewPoint);
}
bool HelloWorld::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto sprite = Sprite::create("HelloWorld.png");
	sprite->setAnchorPoint(Point(0.5, 0.5));
	sprite->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	
	this->addChild(sprite);

	_screenWidth = visibleSize.width;
	_screenHeight = visibleSize.height;
	_tileMap = TMXTiledMap::create("TileMap.tmx");
	addChild(_tileMap);
	/*_tileMap->setAnchorPoint(Point(0.5, 0.5));
	_tileMap->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));*/

	TMXObjectGroup* og= _tileMap->getObjectGroup("Objects");
	ValueMap vm=og->getObject("player");
	int x=vm.at("x").asInt();
	int y = vm.at("y").asInt();
	_player = Sprite::create("player.png");
	_player->setPosition(Point(x,y));
	_player->setAnchorPoint(Point(0.5, 0));
	_tileMap->addChild(_player);
	setViewpointCenter(_player->getPosition());

	EventListenerTouchOneByOne* planeListener = EventListenerTouchOneByOne::create();

	return true;
}
