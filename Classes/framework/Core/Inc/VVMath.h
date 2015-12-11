#pragma once
#ifndef __VVMATH_H__
#define __VVMATH_H__

/*
	Venus Vector Math library. 
	wrapper for sse and neon. 
*/

#if V_SSE || V_NEON
#define VVECTOR_MATH 1
#else 
#define VVECTOR_MATH 0
#endif 


#if VVECTOR_MATH

#if V_SSE
#include <emmintrin.h>

typedef __m128 VREGSITER;	
#else 

#endif 


VCoreApi extern const VREGSITER VVM_ZERO;	// 0.0
VCoreApi extern const VREGSITER VVM_ONE;	// 1.0	
VCoreApi extern const VREGSITER VVM_05;		// 0.5



inline VREGSITER VVMLoad(const float* u)
{
	return _mm_loadu_ps(u);
}


/// store VR to Ptr[0] Ptr[1] Ptr[2] 
inline void VVMStoreFloat3(float* Ptr, const VREGSITER& VR)
{
	union{
		VREGSITER vr;
		float f[4];
	}Temp;
	Temp.vr = VR;
	Ptr[0] = Temp.f[0];
	Ptr[1] = Temp.f[1];
	Ptr[2] = Temp.f[2];
}



inline VREGSITER VVMAdd(VREGSITER a, VREGSITER b)
{
#if V_SSE
	return _mm_add_ps(a,b);
#elif V_NEON

#endif 
}

inline VREGSITER VVMAbs(VREGSITER v)
{
#if V_SSE

#elif V_NEON

#endif 
}

inline VREGSITER VVMNeg(VREGSITER v)
{
	return v;
}

#endif 

#endif 
