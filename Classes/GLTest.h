#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

USING_NS_CC;
class GLTest:public cocos2d::Layer
{
public:
	virtual void visit(Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
	//virtual void visit(Renderer *renderer, const Mat4 &transform, bool transformUpdated) override;
	void onDraw();
};
#endif _APP_DELEGATE_H_
