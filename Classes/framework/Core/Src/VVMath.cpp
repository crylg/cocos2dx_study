#include "VStdAfx.h"
#include "VVMath.h"
#if V_SSE
const VREGSITER VVM_ZERO =_mm_setzero_ps();	// 0.0
const VREGSITER VVM_ONE  = _mm_setr_ps(1.0f,1.0f,1.0f,1.0f);	// 1.0	
const VREGSITER VVM_05 = _mm_setr_ps(0.5f,0.5f,0.5f,0.5f);		// 0.5
#endif 