#include "VStdAfx.h"
#include "VGame.h"

static VGame* Game = NULL;

USING_NS_CC;


VGame::VGame()
	:mDirector(NULL)
	, mGLView(NULL)
	, mDesignSize(1280.0f, 720.0f)
	, mViewportSize(1280.0f, 720.0f)
{
	Game = this;
}


VGame::~VGame()
{
	Game = NULL;
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void VGame::initGLContextAttrs()
{
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };
	GLView::setGLContextAttrs(glContextAttrs);
}

bool VGame::applicationDidFinishLaunching() 
{

	CCLOG("Application Finish Launching...");
	mDirector = Director::getInstance();
	
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {

#if VPLATFORM_WIN32
		glview = GLViewImpl::createWithRect("vGLView", Rect(0,0,1280,720));
		//glview = GLViewImpl::createWithRect("vGLView", Rect(0, 0, 1920, 1080));
	//	glview = GLViewImpl::createWithRect("vGLView", Rect(0,0,1334,750));
//		glview = GLViewImpl::createWithRect("vGLView", Rect(0,0,2560,1440));
		//glview = GLViewImpl::createWithRect("vGLView", Rect(0,0,960.0f,640.0f));
#else 
		glview = GLViewImpl::create("vGLView");
#endif 
		director->setOpenGLView(glview);
	}

	director->setOpenGLView(glview);
	mGLView = glview;

	if (InitGame())
	{
		mDirector->getScheduler()->scheduleUpdate(this, 0, false);
		return true;
	}
	return false;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void VGame::applicationDidEnterBackground() 
{
	OnDeactive();
	

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void VGame::applicationWillEnterForeground() {
	OnActive();
	//Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}

void VGame::update(float DeltaTime)
{
	UpdateGame(DeltaTime);
}