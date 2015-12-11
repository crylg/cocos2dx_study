#ifndef __VPACKED_FLOAT_H__
#define __VPACKED_FLOAT_H__
#include "VMath.h"

class VHalf
{
public:
	static const float SCALE_FACTOR;
	static const float INV_SCALE_FACTOR;
	WORD Value;

	inline VHalf(){}

	inline VHalf(float f):Value(FloatToHalf(f))
	{

	}

	inline VHalf(const VHalf& Other):Value(Other.Value){}
	inline VHalf& operator=(VHalf& v) { Value = v.Value; return *this; }
	inline VHalf& operator=(float f) { Value = FloatToHalf(f); return *this; }

	inline operator float() const { return HalfToFloat(Value); }


	inline float FloatValue() const 
	{
		return HalfToFloat(Value);
	}

	static inline unsigned short FloatToHalf(float f)
	{
		unsigned short h;
		const unsigned int i = *((unsigned int*)&f);
		unsigned int e = (i >> 23) & 0x00ff;
		unsigned int m = i & 0x007fffff;
		if(e <= 112)	// 127 -15
		{
			h = (unsigned short)(((m | 0x00800000) >> (113 - e)) >> 13);
		} else 
		{
			h = (unsigned short)(i >> 13) & 0x3fff;
		}
		h |= (unsigned short)(i >> 16) & 0xc000;	// sign
		return h;
	}


	static inline float HalfToFloat(unsigned short h)
	{
		unsigned int i = (h << 16) & 0x80000000;
		unsigned int em = h & 0x7fff;		// Exponent and Mantissa
		if(em > 0x03ff) 
		{
			i |= (em << 13) + (112 << 23);
		} else 
		{
			unsigned int m = em & 0x03ff;
			if(m != 0) 
			{
				unsigned int e = (em >> 10) & 0x1f;
				while((m & 0x0400) == 0) 
				{
					m <<= 1;
					e--;
				}
				m &= 0x3ff;
				i |= ((e + 113) << 23) | (m << 13);
			}
		}
		return *((float*)&i);
	}

};

class VHalf3
{
public:
	VHalf x,y,z;
	inline VHalf3(){}
	inline VHalf3(const VHalf3& Other):
	x(Other.x),y(Other.y),z(Other.z)
	{

	}

	inline VHalf3(const VVector3& Other):x(Other.x),y(Other.y),z(Other.z)
	{

	}

	inline void operator=(const VVector3& Other)
	{
		x.Value = VHalf::FloatToHalf(Other.x);
		y.Value = VHalf::FloatToHalf(Other.y);
		z.Value = VHalf::FloatToHalf(Other.z);
	}
};


#endif 