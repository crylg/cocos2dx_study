#include "VStdAfx.h"
#include "VView.h"
#include "VViewManager.h"

USING_NS_CC;


cocos2d::Node* VView::LoadView(const char* FileName)
{
	VASSERT(FileName && FileName[0]);
	Node* RootNode = CSLoader::createNode(FileName);
	return RootNode;
}

cocostudio::timeline::ActionTimeline* VView::LoadAction(const char* FileName)
{
	VASSERT(FileName && FileName[0]);
	cocostudio::timeline::ActionTimeline* timelineAction = CSLoader::createTimeline(FileName);
	return timelineAction;
}


VView::VView()
{

}


VView::~VView()
{
}

void VView::DeleteThis()
{
	VDelete this;
}

BOOL VView::LoadViewFrom(const char* FileName)
{
	VASSERT(FileName && FileName[0]);
	Node* RootNode = CSLoader::createNode(FileName);
	if (RootNode == NULL)
	{
		return FALSE;
	}

	if (!OnLoadView(RootNode))
	{
		RootNode->release();
		return FALSE;
	}

	VSize ContentSize = RootNode->getContentSize();
	RootNode->setContentSize(VSize(960.0f, 640.0f));
	this->addChild(RootNode);
	this->autorelease();
	return TRUE;
}

void VView::DoLayout(Node* node, LayoutType type)
{
	Size winsize = Director::getInstance()->getVisibleSize();
	if (type == CENTER)
	{
		node->setPosition((winsize - node->getContentSize())/2);
	}
}

BOOL VView::OnLoadView(cocos2d::Node* Root)
{
	return TRUE;
}




