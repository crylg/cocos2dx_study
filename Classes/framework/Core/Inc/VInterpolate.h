#pragma once
#ifndef __VINTERPOLATE_H__
#define __VINTERPOLATE_H__
//https://github.com/warrenm/AHEasing/blob/master/AHEasing/easing.c
//http://www.robertpenner.com/easing/

enum EVEasingType
{
	VET_IN = 0,
	VET_OUT,
	VET_IN_OUT,
};

enum EVEasingFunction
{
	VEF_LINEAR = 0,
	VEF_QUAD_IN,
	VEF_QUAD_OUT,
	VEF_QUAD_IN_OUT,
};


/**
 *  linear easing (T' = T)
 *	
 */
struct VEasingLinear
{
	static inline float EaseIn(float Time)
	{
		return Time;
	}
	static inline float EaseOut(float Time)
	{
		return Time;
	}
	static inline float EaseInOut(float Time)
	{
		return Time;
	}
};

/**
 *  Quadratic easing (T' = T ^ 2) 
 *	// Note: pow based easing. 
 */
struct VEasingQuad
{
	static inline float EaseIn(float Time)
	{
		return Time * Time;
	}
	static inline float EaseOut(float Time)
	{
		return Time*(2.0f - Time);;
	}
	static inline float EaseInOut(float Time)
	{
		if (Time < 0.5f)
		{
			Time = 2.0f * Time * Time; 
		}else
		{
			Time = (-2 * Time * Time) + (4 * Time) - 1;
		}
		return Time;
	}
};


/**
 *  Cubic easing (T' = T ^ 3) 
 *	
 */
struct VEasingCubic
{
	static inline float EaseIn(float Time)
	{
		return Time * Time * Time;
	}
	static inline float EaseOut(float Time)
	{
		Time = Time - 1;
		return Time * Time * Time + 1;
	}
	static inline float EaseInOut(float Time)
	{
		if (Time < 0.5f)
		{
			Time = 4.0f * Time * Time * Time; 
		}else
		{
			Time = Time * 2.0f - 2.0f;
			Time = Time * Time * Time * 0.5f + 1.0f;
		}
		return Time;
	}
};




//////////////////////////////////////////////////////////////////////////

/**
 *  Elastic easing (T' = sin(13pi/2*T)*pow(2, 10 * (T - 1))) 
 *	类似弹簧的来回阻尼震动,直到停止. 
 */

struct VEaseElastic
{
	static inline float EaseIn(float Time)
	{
		// modify from the robertpenner's implemention. 
		//Time = sin(13.0f * M_PI_2 * (Time - 1.075f)) * powf(2.0f, 10.0f * (Time - 1.0f));
		return sin(13.0f * VPI * Time) * powf(2.0f, 10.0f * (Time - 1.0f));
	}
	static inline float EaseOut(float Time)
	{
		return sin(-13.0f * VPI * (Time + 1)) * pow(2, -10 * Time) + 1;
	}
	static inline float EaseInOut(float Time)
	{
		if (Time < 0.5f)
		{
			Time = 0.5f * sin(13 * VPI * (2 * Time)) * pow(2, 10 * ((2 * Time) - 1));
		}else
		{
			Time = 0.5f * (sin(-13 * VPI * ((2 * Time - 1) + 1)) * pow(2, -10 * (2 * Time - 1)) + 2);
		}
		return Time;
	}

};


/**
 *	Usage float Value = VEaseIn<VEasingCubic>(1.0f, 2.0f, 0.2f); 
 *	
 */
template<typename EasingFunction, typename T>
inline T VEaseIn(const T& From, const T& To, float Time)
{
	float Factor = EasingFunction::EaseIn(Time);
	return VLerp(From, To, Factor);
}


template<typename T>
inline T VEaseLinear(const T& From, const T& To, float Time)
{
	return VEaseIn<VEasingLinear>(From, To, Time);
}


#endif 
