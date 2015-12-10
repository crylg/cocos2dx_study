#ifndef  __GLTesting_H_
#define  __GLTesting_H_

#include "cocos2d.h"

USING_NS_CC;

class GLTestingLayer : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void menuCloseCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(GLTestingLayer);
	void graySprite(Sprite * sprite);
	//virtual void visit(Renderer* renderer, const kmMat4 &parentTransform, bool parentTransformUpdated) override;
	void onDraw();
private:
	CustomCommand _command;
};
#endif // ! __GLTesting_H_
