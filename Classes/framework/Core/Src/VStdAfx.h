/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	VStdAfx.h
//	Created: 2014-1-18   13:35
//
**********************************************************************************/
#pragma once

#ifndef VCoreApi

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

#include "VBase.h"
#include "VMath.h"
#include "VProfiler.h"
#include "VString.h"
#include "VThreading.h"