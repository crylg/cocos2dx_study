#pragma once
#ifndef __VGAME_H__
#define __VGAME_H__

#include "VAppComponent.h"



class VCoreApi VGame : public cocos2d::Application
{
public:
	VGame();
	virtual ~VGame();
	inline const VSize& GetDesignSize() const { return mDesignSize;  }
	inline const VSize& GetViewportSize() const { return mViewportSize; }
	void update(float DeltaTime);

	inline VAppComponent* GetComponent(UINT ComponentName);
	virtual BOOL InitGame() = 0;
	virtual void ShutdownGame() = 0;
protected:
	
	virtual void UpdateGame(float DeltaTime) = 0;
	virtual void OnActive() = 0;
	virtual void OnDeactive() = 0;

protected:
	cocos2d::Director*  mDirector;
	cocos2d::GLView* mGLView;
	VSize mDesignSize;
	VSize mViewportSize;
	// cocos2d application inmplememt
protected:
	virtual void initGLContextAttrs();
	/**
	@brief    Implement Director and Scene init code here.
	@return true    Initialize success, app continue.
	@return false   Initialize failed, app terminate.
	*/
	virtual bool applicationDidFinishLaunching();

	/**
	@brief  The function be called when the application enter background
	@param  the pointer of the application
	*/
	virtual void applicationDidEnterBackground();

	/**
	@brief  The function be called when the application enter foreground
	@param  the pointer of the application
	*/
	virtual void applicationWillEnterForeground();
};


#endif 

