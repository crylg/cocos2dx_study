#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;
class HelloWorld : public cocos2d::Layer
{
public:
	
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
	
private:
	
	void setViewpointCenter(cocos2d::Vec2 position);
	void graySprite(cocos2d::Sprite * sprite);
	cocos2d::TMXTiledMap* _tileMap;
	cocos2d::Sprite* _player;
	int _screenWidth;
	int _screenHeight;
};

#endif // __HELLOWORLD_SCENE_H__
