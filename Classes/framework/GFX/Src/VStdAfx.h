// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "VCore.h"


//#ifndef VCoreApi
#if VPLATFORM_WIN32
#undef VCoreApi
#ifndef V_LIBS
#ifdef _WIN32
#define VCoreApi	__declspec(dllexport)
#else
#define VCoreApi	__attribute__((visibility("default")))
#endif
#else 
#define VCoreApi
#endif 
#endif 
//#endif 


#include "cocos2d.h"

#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "CCWrapper.h"