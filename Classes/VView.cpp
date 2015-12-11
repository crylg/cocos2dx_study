#include "stdafx.h"
#include "VView.h"
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
USING_NS_CC;
Node* VView::LoadView(const char* FileName)
{
	VASSERT(FileName && FileName[0]);
	Node* node = CSLoader::createNode(FileName);
	return node;
}

//Node* VView::getSkin()
//{
//	return _skin;
//}
//VView::~VView()
//{
//	if (_skin->getParent() != nullptr)
//	{
//		_skin->removeFromParent();
//	}
//}