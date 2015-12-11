#ifndef __VCOLOR_H__
#define __VCOLOR_H__
#pragma once
#include "VMath.h"
#pragma warning(push)
#pragma warning(disable:4244)
class VColorI;

class VCoreApi VColorF
{
public:
	static const VColorF WHITE;
	static const VColorF BLACK;

	float r,g,b,a;

	inline VColorF(){}
	inline VColorF(int i):r(1.0f),g(1.0f),b(1.0f),a(1.0f){}

	inline VColorF(float R, float G, float B, float A):r(R),g(G),b(B),a(A){}
	inline ~VColorF(){}

	inline float Luminance() const 
	{
		return r*0.30f + g*0.59f + b*0.11f;
	}

	inline UINT GetRGBA() const 
	{
		union{
			struct {
				BYTE r,g,b,a;
			};
			UINT ColorValue;
		}RGBA;
		RGBA.r = (BYTE)VClamp<int>(r * 255.0f, 0, 255);
		RGBA.g = (BYTE)VClamp<int>(g * 255.0f, 0, 255);
		RGBA.b = (BYTE)VClamp<int>(b * 255.0f, 0, 255);
		RGBA.a = (BYTE)VClamp<int>(a * 255.0f, 0, 255);
		return RGBA.ColorValue;
	}
};


// Note : we use UB4N for render internal, not DWORD COLOR. 
// UB4 : packed in ABGR and DWORD packed in ARGB
// UB4 worked in D3D, OGL,D3D10+ but DWORD need some specify operation in D3D10+ 
class VColorI
{
public: 
	union{
		struct {
			BYTE r,g,b,a;
		};
		struct 
		{
			BYTE b,g,r,a;
		}ARGB;
		UINT ColorValue;
	};
	inline VColorI(){}
	inline VColorI(BYTE R, BYTE G, BYTE B, BYTE A):r(R),g(G),b(B),a(A){}
	inline VColorI(const VColorI& Color):ColorValue(Color.ColorValue){}
	inline UINT GetARGB() const
	{
		VColorI ARGBColor(*this);
		ARGBColor.ConvertToARGB();
		return ARGBColor.ColorValue;
	}

	inline void ConvertToARGB()
	{
		VSwap(r, b);
	}
};

typedef VColorF VColor;

#pragma warning(pop)


#endif 
