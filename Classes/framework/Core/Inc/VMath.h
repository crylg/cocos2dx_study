/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2008 Venus.PK, All Right Reversed.
//	VMath.h
//	Created: 2005-7-18   4:30
//
**********************************************************************************/
#ifndef __VMATH_H__
#define __VMATH_H__

#include "VBase.h"
#include "VVMath.h"

#pragma warning(push)
#pragma warning(disable : 4244)
// V_SSE
// V_SSE2
// V_NEON
// V_FPU

#if V_SSE
#include <xmmintrin.h>
#elif V_NEON
#include <arm_neon.h>
#endif

/*
 */
#if V_SSE
	#define _MM_PERM2_X		0
	#define _MM_PERM2_Y		1
	#define _MM_PERM2_Z		2
	#define _MM_PERM2_W		3
	#define VSHUFFLE_MASK(X,Y,Z,W) _MM_SHUFFLE(_MM_PERM2_ ## W,_MM_PERM2_ ## Z,_MM_PERM2_ ## Y,_MM_PERM2_ ## X)
	#define _MM_SWIZZLE(V,X,Y,Z,W) _mm_shuffle_ps(V,V,VSHUFFLE_MASK(X,Y,Z,W))
	extern VCoreApi __m128 VSSE_CR1_11_1;
	extern VCoreApi __m128 VSSE_CR11_1_1;
	extern VCoreApi __m128 VSSE_CR_111_1;

inline __m128 _mm_rcp_ss_nr(__m128 v) 
{
	__m128 iv = _mm_rcp_ss(v);
	iv = _mm_sub_ss(_mm_add_ss(iv,iv),_mm_mul_ss(v,_mm_mul_ss(iv,iv)));
	return _mm_sub_ss(_mm_add_ss(iv,iv),_mm_mul_ss(v,_mm_mul_ss(iv,iv)));
}
#elif V_NEON
	#if defined(VPLATFORM_WINRT) || defined(VPLATFORM_IOS)
		#define NEON_UINT2(X,Y) vset_lane_u32(Y,vdup_n_u32(X),1)
		#define NEON_UINT4(X,Y,Z,W) vsetq_lane_u32(W,vsetq_lane_u32(Z,vsetq_lane_u32(Y,vdupq_n_u32(X),1),2),3)
	#else
		#define NEON_UINT2(X,Y) { X, Y }
		#define NEON_UINT4(X,Y,Z,W) { X, Y, Z, W }
	#endif
#endif


#define VSQR(x)			((x)*(x))
#define VEPS			((float)0.000001f)
#define VINFINITY		(1e+8f)
#define VPI			((float)3.1415926535897932f)
#define V2PI			((float)6.2831853071795865f)
#define VHALF_PI		((float)1.5707963267948966f)
#define VDEG_RAD(x)		(x*0.017453292519943296f)
#define VRAD_DEG(x)		(x*57.29577951308232088f)
#define VLOG2			0.693147181f
#define VLOG2_INV		1.442695041f

template<typename T> 
inline T VAbs(const T Value)		{ return Value >= (T)0 ? Value : -Value;}
template<typename T> 
inline T VMax( const T& A, const T& B )			{ return (A>=B) ? A : B;}
template<typename T> 
inline T VMin( const T& A, const T& B )			{ return (A<=B) ? A : B;}

inline float VSin(float fValue)		{ return sinf(fValue); }
// Note. 抛弃了fast_sin fast_cos 的实现, 局限太大, 容易导致误差.
inline float VCos(float fValue)		{ return cosf(fValue); }
inline float VACos(float fValue)	{ return acosf(fValue); }
inline float VASin(float fValue)	{ return asinf(fValue); }
inline void VSinCos(float angle, float& s, float& c)
{
#if defined(_WIN32) && defined(VARCH_X86)
	__asm {
		fld angle
		fsincos
		mov edx, s
		mov ecx, c
		fstp dword ptr [ecx]
		fstp dword ptr [edx]
	}
#elif defined(_LINUX) && !defined(ARCH_ARM)
	double res_0,res_1;
	asm volatile("fsincos" : "=t"(res_0), "=u"(res_1) : "0"(angle));
	s = (float)res_1;
	c = (float)res_0;
#else
	s = VSin(angle);
	c = VCos(angle);
#endif
}
inline float VTan(float fValue)		{ return tanf(fValue); }
inline float VATan(float fValue)	{ return atanf(fValue); }
inline float VATan2(float x, float y)	{ return atan2f(x, y); }
inline float VExp(float fValue)			{ return expf(fValue); }
inline int VFloor(float v)		
{ 
#if V_SSE
	return _mm_cvt_ss2si(_mm_set_ss(v + v - 0.5f) ) >> 1;
#else
	return (int)floorf(v); 
#endif 
}
inline int VCeil(float v)		
{
#if V_SSE
	return -(_mm_cvt_ss2si(_mm_set_ss(-0.5f - (v + v))) >> 1);
#else 
	return (int)ceilf(v); 
#endif 
}
inline float VFMod(float x, float y)	{ return fmodf(x, y); } 
inline float VPow(float fBase, float fExp)	{return powf(fBase, fExp); }
inline float VSqrt(float v)
{
#if V_SSE
	_mm_store_ss(&v,_mm_sqrt_ss(_mm_set_ss(v)));
	return v;
#else
	return sqrtf(v);
#endif
}

inline float VInvSqrt(float v)	
{
	// TODO , 这里可以用SSE 优化, 
	// _mm_store_ss(&v,_mm_rsqrt_ss(_mm_set_ss(v))); 但是结果并不精确. 需要牛顿插值. 
	// Algorithm for fast inverse square root from Dave Eberly's WildMagic code.
	// See http://www.wild-magic.com/Web/Documents/FastInverseSqrt.pdf for
	// implementation discussion.

	return 1.0f / VSqrt(v);	
}

// round a float value to nearest int.
inline int VRound(float v)
{
#if VPLATFORM_WIN32 && defined(VARCH_X86)
	int i;
	__asm {
		fld v
			fistp i
	}
	return i;
#elif V_SSE
	return _mm_cvt_ss2si(_mm_load_ss(&v));
#else 
	return (int)(v+0.5f);
#endif 
}

// get a float value 's faction number
inline float VFrac(float v) { return v - VFloor(v);}

template<typename T> 
inline void VSwap(T& A, T& B )					{ T Temp = B; B = A; A = Temp;}

template<typename T> 
inline T VClamp(const T& Value, const T& Min, const T& Max) { return Value < Min ? Min : ( Value > Max ? Max : Value) ; }


inline int LSign(float fValue)
{
	//(*(DWORD*)&fValue) >= 0x80000000 ? -1 : 1
	return ( fValue > 0.0f ? 1 : ( fValue < 0.0f ? -1 : 0 ) );
}

// is Power Of Two
inline BOOL VIsPOT(UINT Value)	{return (Value & (Value - 1)) == 0;	}
// return next pow of two
inline UINT VNextPOT(UINT Value)
{
	Value--;
	Value |= Value >> 1;
	Value |= Value >> 2;
	Value |= Value >> 4;
	Value |= Value >> 8;
	Value |= Value >> 16;
	Value++;
	return Value;
}

template<typename T>
inline T VAlign(T Value, UINT Alignment )
{
	return (Value + Alignment - 1) & ~(Alignment-1);
}

template<typename T>
inline bool VIsAlign(T Value, UINT Alignment)
{
	return (Value & (T)(Alignment-1)) == 0;
}

inline float VLog(float Value)
{
	return (float)logf(Value);
}

inline int VLog2(int Value)
{
	int ret = 0;
	if(Value >= 1 << 16) { Value >>= 16; ret |= 16; }
	if(Value >= 1 << 8) { Value >>= 8; ret |= 8; }
	if(Value >= 1 << 4) { Value >>= 4; ret |= 4; }
	if(Value >= 1 << 2) { Value >>= 2; ret |= 2; }
	if(Value >= 1 << 1) { ret |= 1; }
	return ret;
}

inline float VLog2(float Value)
{
	return VLog(Value) * VLOG2_INV;
}




union VIntFloat 
{
	inline VIntFloat() { }
	inline VIntFloat(int i) : i(i) { }
	inline VIntFloat(float f) : f(f) { }
	inline VIntFloat(unsigned int ui) : ui(ui) { }
	int i;
	float f;
	unsigned int ui;
};


//////////////////////////////////////////////////////////////////////////
// Fast math Only support IEEE float 
//////////////////////////////////////////////////////////////////////////
inline float VLog2Fast(float x)
{
	VIntFloat fi(x);
	VIntFloat mx((fi.i & 0x007FFFFF) | 0x3f000000);
	x = (float)fi.i * 1.1920928955078125e-7f;
	return x - 124.22551499f - 1.498030302f * mx.f  - 1.72587999f / (0.3520887068f + mx.f);
}

inline float VLogFast (float x)
{
	return 0.69314718f * VLog2Fast(x);
}


inline float VLog2FastEx(float x)
{
	VIntFloat fi(x);
	x = (float)fi.i * 1.1920928955078125e-7f;
	return x - 126.94269504f;
}

inline float VLogFastEx(float x)
{
	VIntFloat fi(x);
	x = (float)fi.i * 8.2629582881927490e-8f;
	return x - 87.989971088f;
}


// return 2^x 
inline float VExp2Fast(float x)
{
#if 0
	int i = ftoi(v - 0.5f);
	v = v - itof(i);
	return IntFloat((i + 127) << 23).f * (((((0.0018775767f * v + 0.0089893397f) * v + 0.055826318f) * v + 0.24015361f) * v + 0.69315308f) * v + 0.99999994f);
#endif 
	//https://code.google.com/p/fastapprox/source/browse/trunk/fastapprox/src/fastexp.h

	float offset = (x < 0) ? 1.0f : 0.0f;
	float clipp = (x < -126) ? -126.0f : x;
	int w = (int)clipp;
	float z = clipp - w + offset;
	VIntFloat ret;
	ret.ui = static_cast<unsigned int>((1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z));
	return ret.f;
}

inline float VExp2FastEx (float p)
{
	float clipp = (p < -126) ? -126.0f : p;
	VIntFloat ret;
	ret.ui = static_cast<unsigned int>((1 << 23) * (clipp + 126.94269504f));
	return ret.f;
}

inline float VExpFast (float p)
{
	return VExp2Fast (1.442695040f * p);
}


inline float VExpFastEx (float p)
{
	return VExpFast (1.442695040f * p);
}

inline float VPowFast(float x, float p)
{
	return VExp2Fast(p * VLog2Fast(x));
}

inline float VPowFastEx (float x, float p)
{
	return VExp2FastEx (p * VLog2FastEx (x));
}

//////////////////////////////////////////////////////////////////////////
//	vector math
//////////////////////////////////////////////////////////////////////////
class VVector3;
class VVector4;
//class VMatrix3X3;		
class VMatrix4X4;
class VQuat;

inline BOOL VEqual(float f0, float f1, float Eps = VEPS);
inline BOOL VEqual(const VVector3& v0, const VVector3& v1, float Eps = VEPS);
inline BOOL VEqual(const VVector4& v0, const VVector4& v1, float Eps = VEPS);
inline BOOL VEqual(const VQuat& q0, const VQuat& q1, float Eps = VEPS);

inline VVector3& VVec3Transform(VVector3& Out, VVector3& V, VMatrix4X4& Mat);

// 计算逆矩阵
inline const VMatrix4X4& VMatInverse(VMatrix4X4& Out, const VMatrix4X4& Mat);


class VCoreApi VVector2
{
public:
	static const VVector2 ZERO;
	static const VVector2 ONE;

	float x,y;
	inline VVector2() { }
	inline VVector2(const VVector2 &v) : x(v.x), y(v.y) { }
	inline VVector2(float x,float y) : x(x), y(y) { }
	explicit inline VVector2(int v) : x((float)v), y((float)v) { }
	explicit inline VVector2(float v) : x(v), y(v) { }
	explicit inline VVector2(const float *v) : x(v[0]), y(v[1]) { }

	inline operator float*()							{ return (float*)this; }
	inline operator const float*() const				{ return (float*)this; }
	inline float& operator[](int index)					{ return ((float*)this)[index]; }
	inline const float& operator[](int index) const		{ return ((const float*)this)[index]; }

	inline VVector2 operator-() const					{ return VVector2(-x,-y);	}

	inline void operator*=(float v)						{ x *= v; y *= v; }
	inline void operator*=(const VVector2 &v)			{ x *= v.x; y *= v.y; }
	inline void operator/=(float v)						{ float iv = 1.0f / v;	x *= iv; y *= iv; }
	inline void operator/=(const VVector2 &v)			{ x /= v.x; y /= v.y; }
	inline void operator+=(const VVector2 &v)			{ x += v.x; y += v.y; }
	inline void operator-=(const VVector2 &v)			{ x -= v.x; y -= v.y; }

	inline VVector2 operator+(const VVector2& v) const	{return VVector2(x + v.x, y + v.y);}
	inline VVector2 operator+(float v) const			{return VVector2(x + v, y + v);}
	inline VVector2 operator-(const VVector2& v) const	{return VVector2(x - v.x, y - v.y);}
	inline VVector2 operator-(float v) const			{return VVector2(x - v, y - v);}
	inline VVector2 operator*(const VVector2& v) const	{return VVector2(x * v.x, y * v.y);}
	inline VVector2 operator*(float v) const			{return VVector2(x * v, y * v);}
	inline VVector2 operator/(const VVector2& v) const	{return VVector2(x / v.x, y / v.y);}
	inline VVector2 operator/(float v) const			{const float iv = 1.f/v; return VVector2(x * iv, y * iv);}

	inline void Set(float v)							{x = v; y = v;}
	inline void Set(float x_,float y_)			{x = x_; y = y_; }
	inline void Set(const float *v)						{x = v[0]; y = v[1]; }
	inline float LengthSqr() const						{return x * x + y * y;}
	inline float Length() const							{return VSqrt(x * x + y * y);}
	inline void Normalize() 
	{
		float il = VInvSqrt(x * x + y * y);
		x *= il; y *= il;;
	}
	inline BOOL IsNomalized() const						{ return VAbs((LengthSqr() - 1.0f)) < 0.01f; }

	 inline float Dot(const VVector2&rhs) const {return x*rhs.x + y*rhs.y;}
};

class VCoreApi VVector3
{
public:
	static const VVector3 XAXIS;
	static const VVector3 YAXIS;
	static const VVector3 ZAXIS;
	static const VVector3 NEG_XAXIS;
	static const VVector3 NEG_YAXIS;
	static const VVector3 NEG_ZAXIS;
	static const VVector3 ZERO;
	static const VVector3 ONE;

	float x,y,z;
	inline VVector3() { }
	inline VVector3(float _x,float _y,float _z) : x(_x), y(_y), z(_z)	{}
	explicit inline VVector3(float v) : x(v), y(v), z(v) {}
	explicit inline VVector3(const VVector2 &v) : x(v.x), y(v.y), z(0.0f){ }
	explicit inline VVector3(const VVector4 &v);
	explicit inline VVector3(const float *v) : x(v[0]), y(v[1]), z(v[2]){ }

	inline operator float*()							{ return (float*)this; }
	inline operator const float*() const				{ return (float*)this; }
	inline float& operator[](int index)					{ return ((float*)this)[index]; }
	inline const float& operator[](int index) const		{ return ((const float*)this)[index]; }

	inline VVector3 operator-() const					{ return VVector3(-x,-y,-z);	}

	// VVector3& operator*= += /=  return value is not common use , so we return void!!
	// vec3_1 = vec3_0 += ofs;  ---->  vec3_0 += ofs; vec3_1 = vec3_0;  
	inline void operator*=(float v)						{ x *= v; y *= v; z *= v; }
	inline void operator*=(const VVector3 &v)			{ x *= v.x; y *= v.y; z *= v.z;}
	inline void operator/=(float v)						{ float iv = 1.0f / v;	x *= iv; y *= iv; z *= iv;}
	inline void operator/=(const VVector3 &v)			{ x /= v.x; y /= v.y; z /= v.z; }
	inline void operator+=(const VVector3 &v)			{ x += v.x; y += v.y; z += v.z; }
	inline void operator-=(const VVector3 &v)			{ x -= v.x; y -= v.y; z -= v.z; }

	inline VVector3 operator+(const VVector3& v) const	{return VVector3(x + v.x, y + v.y, z + v.z);}
	inline VVector3 operator+(float v) const			{return VVector3(x + v, y + v, z + v);}
	inline VVector3 operator-(const VVector3& v) const	{return VVector3(x - v.x, y - v.y, z - v.z );}
	inline VVector3 operator-(float v) const			{return VVector3(x - v, y - v, z - v);}
	inline VVector3 operator*(const VVector3& v) const	{return VVector3(x * v.x, y * v.y, z * v.z);}
	inline VVector3 operator*(float v) const			{return VVector3(x * v, y * v, z * v);}
	inline VVector3 operator/(const VVector3& v) const	{return VVector3(x / v.x, y / v.y, z / v.z);}
	inline VVector3 operator/(float v) const			{const float iv = 1.f/v; return VVector3(x * iv, y * iv, z * iv);}

	// dot
	inline float operator|(const VVector3& v) const		{ return x*v.x + y*v.y + z*v.z;}
	// cross
	inline VVector3 operator^(const VVector3& v) const	{ return VVector3(y * v.z - z * v.y, z * v.x - x * v.z,x * v.y - y * v.x);}
	//点乘
	inline float Dot( const VVector3& v ) const			{ return x*v.x + y*v.y + z*v.z; }
	// 叉乘
	inline VVector3 Cross( const VVector3& v ) const	{ return VVector3(y * v.z - z * v.y, z * v.x - x * v.z,x * v.y - y * v.x); }

	inline void Set(float v)							{x = v; y = v; z = v;}
	inline void Set(float x_,float y_,float z_)			{x = x_; y = y_; z = z_;}
	inline void Set(const float *v)						{x = v[0]; y = v[1]; z = v[2];}
	inline float LengthSqr() const						{return x * x + y * y + z * z;}
	inline float Length() const							{return VSqrt(x * x + y * y + z * z);}
	inline void Normalize() 
	{
		float il = VInvSqrt(x * x + y * y + z * z);
		x *= il; y *= il; z *= il;
	}
	inline BOOL IsNomalized() const						{ return VAbs((LengthSqr() - 1.0f)) < 0.01f; }
	//! 计算与this正交的2条基底坐标轴(不定解), 返回值为单位矢量
	void GetBaseAxis(VVector3& vAxis1, VVector3& vAxis2);

	//! 将this绕vAxis 旋转 fAngle 
	inline VVector3 RotateByAxisAngle(const VVector3& vAxis, float fAngle) const;

	//! 将this 经 Normal 反射,确保Normal 为单位矢量
	inline VVector3 ReflectionVector(const VVector3& Normal) const
	{
		return *this - Normal * (2.f * ( *this | Normal));
	}
	//! 将矢量映射到A矢量上. 确保A矢量为单位矢量
	inline VVector3 ProjectOnTo( const VVector3& v ) const 
	{ 
		return v*(*this | v); 
	}
};

// vec4 or plane
VALIGNED_PREFIX(16) class VVector4
{
public:

	VCoreApi static const VVector4 ZERO;
	VCoreApi static const VVector4 ONE;

	union {
		struct {
			float x,y,z,w;
		};
#ifdef V_SSE
		__m128 vec;
#elif V_NEON
		float32x4_t vec;
#endif
	};
	inline VVector4() { }
	inline VVector4(const VVector3 &v,float w) : x(v.x), y(v.y), z(v.z), w(w) { }
	inline VVector4(float x,float y,float z,float w) : x(x), y(y), z(z), w(w) { }
	explicit inline VVector4(float v) : x(v), y(v), z(v), w(v) { }
	explicit inline VVector4(const VVector3 &v) : x(v.x), y(v.y), z(v.z), w(1.0f) { }
	explicit inline VVector4(const float *v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) { }

	inline operator float*()							{ return (float*)this; }
	inline operator const float*() const				{ return (float*)this; }
	inline float& operator[](int index)					{ return ((float*)this)[index]; }
	inline const float& operator[](int index) const		{ return ((const float*)this)[index]; }

	inline VVector4 operator-() const					{ return VVector4(-x,-y,-z,-w);	}

	inline void operator*=(float v)						{ x *= v; y *= v; z *= v; w*=v;}
	inline void operator*=(const VVector4 &v)			{ x *= v.x; y *= v.y; z *= v.z; w*=v.w; }
	inline void operator/=(float v)						{ float iv = 1.0f / v;	x *= iv; y *= iv; z *= iv; w*=iv;}
	inline void operator/=(const VVector4 &v)			{ x /= v.x; y /= v.y; z /= v.z; w /= v.w;}
	inline void operator+=(const VVector4 &v)			{ x += v.x; y += v.y; z += v.z; w+= v.w;}
	inline void operator-=(const VVector4 &v)			{ x -= v.x; y -= v.y; z -= v.z; w-= v.w;}

	inline VVector4 operator+(const VVector4& v) const	{return VVector4(x + v.x, y + v.y, z + v.z, w + v.w);}
	inline VVector4 operator+(float v) const			{return VVector4(x + v, y + v, z + v, w + v);}
	inline VVector4 operator-(const VVector4& v) const	{return VVector4(x - v.x, y - v.y, z - v.z, w - v.w);}
	inline VVector4 operator-(float v) const			{return VVector4(x - v, y - v, z - v, w - v);}
	inline VVector4 operator*(const VVector4& v) const	{return VVector4(x * v.x, y * v.y, z * v.z, w * v.w);}
	inline VVector4 operator*(float v) const			{return VVector4(x * v, y * v, z * v, z * v);}
	inline VVector4 operator/(const VVector4& v) const	{return VVector4(x / v.x, y / v.y, z / v.z, w / v.w);}
	inline VVector4 operator/(float v) const			{const float iv = 1.f/v; return VVector4(x * iv, y * iv, z * iv, w * iv);}

	//点乘
	inline float Dot3(const VVector4& v) const			{ return x*v.x + y*v.y + z*v.z; }
	inline float Dot(const VVector4& v) const			{ return x*v.x + y*v.y + z*v.z + w*v.w; }
	inline float Dot3(const VVector3& v) const			{ return x*v.x + y*v.y + z*v.z;}

	// plane distance.
	inline float Dot(const VVector3& v) const			{ return x*v.x + y*v.y + z*v.z + w; }

	// plane equation 叉乘 (for plane normal.)
	inline VVector3 Cross( const VVector3& v ) const	{ return VVector3(y * v.z - z * v.y, z * v.x - x * v.z,x * v.y - y * v.x); }

	inline void Set(float v)							{x = v; y = v; z = v;}
	inline void Set(float x_,float y_,float z_, float w_)			{x = x_; y = y_; z = z_; w = w_;}
	inline void Set(const float *v)						{x = v[0]; y = v[1]; z = v[2]; w = v[3];}
	inline void Set(const VVector3 &v, float w_ = 1.0f) {x = v.x; y = v.y; z = v.z; w = w_;}

	// plane equation
	inline float LengthSqr() const						{return x * x + y * y + z * z;}
	inline float Length() const							{return VSqrt(x * x + y * y + z * z);}
	inline void Normalize3() 
	{
		float il = VInvSqrt(x * x + y * y + z * z);
		x *= il; y *= il; z *= il;
	}

	// as plane normalize!!
	inline void Normalize() 
	{
		float il = VInvSqrt(x * x + y * y + z * z);
		x *= il; y *= il; z *= il; w *= il;
	}

	inline BOOL IsNomalized() const						{ return VAbs((LengthSqr() - 1.0f)) < 0.01f; }
}VALIGNED_SUFFIX(16);


// see ATI Developer Site : QuantizedVertexNormals 
// 我们不使用10,10,10 格式, 只采用 8 8 8, 
class VByteVector
{
public:
	union
	{
		struct{
			BYTE x;
			BYTE y;
			BYTE z;
			BYTE w;
		};
		UINT Packed;
	};
	inline VByteVector():Packed(0){}
	inline VByteVector( UINT InPacked ):Packed(InPacked) {}
	inline VByteVector(const VVector3& InVector) 
	{ 
		*this = InVector; 
	}

	inline void operator = (const VVector3& Vec)
	{
		// map [-1 1] ---> [0 255]
		x = VClamp<INT>((INT)(Vec.x * 127.5f + 127.5f),0,255);
		y = VClamp<INT>((INT)(Vec.y * 127.5f + 127.5f),0,255);
		z = VClamp<INT>((INT)(Vec.z * 127.5f + 127.5f),0,255);
	}
	inline operator VVector3() const
	{
		return VVector3(x*0.007843137254901961f - 1.0f, y*0.007843137254901961f - 1.0f, z*0.007843137254901961f - 1.0f);
	}
	inline BOOL operator==(const VByteVector& rhs) const
	{
		if(Packed != rhs.Packed)
			return FALSE;
		return TRUE;
	}
};


/// int vector2 
class VVector2i
{
public:
	int x,y;
	inline VVector2i() { }
	inline VVector2i(const VVector2i& v) : x(v.x), y(v.y) { }
	inline VVector2i(int x,int y) : x(x), y(y) { }
	explicit inline VVector2i(const VVector2& fv):x((int)fv.x), y((int)fv.y){}
	explicit inline VVector2i(const int *v) : x(v[0]), y(v[1]) { }

	inline operator int*()							{ return (int*)this; }
	inline operator const int*() const				{ return (int*)this; }
	inline int& operator[](int index)					{ return ((int*)this)[index]; }
	inline const int& operator[](int index) const		{ return ((const int*)this)[index]; }

	inline VVector2i operator-() const					{ return VVector2i(-x,-y);	}

	inline void operator*=(float v)						{ x *= v; y *= v; }
	inline void operator*=(const VVector2i &v)			{ x *= v.x; y *= v.y; }
	inline void operator/=(float v)						{ float iv = 1.0f / v;	x *= iv; y *= iv; }
	inline void operator/=(const VVector2i &v)			{ x /= (float)v.x; y /= (float)v.y; }
	inline void operator+=(const VVector2i &v)			{ x += v.x; y += v.y; }
	inline void operator-=(const VVector2i &v)			{ x -= v.x; y -= v.y; }

	inline VVector2i operator+(const VVector2i& v) const	{return VVector2i(x + v.x, y + v.y);}
	inline VVector2i operator+(float v) const			{return VVector2i(x + v, y + v);}
	inline VVector2i operator-(const VVector2i& v) const	{return VVector2i(x - v.x, y - v.y);}
	inline VVector2i operator-(float v) const			{return VVector2i(x - v, y - v);}
	inline VVector2i operator*(const VVector2i& v) const	{return VVector2i(x * v.x, y * v.y);}
	inline VVector2i operator*(float v) const			{return VVector2i(x * v, y * v);}
	inline VVector2i operator/(const VVector2i& v) const	{return VVector2i(x / v.x, y / v.y);}
	inline VVector2i operator/(float v) const			{const float iv = 1.f/v; return VVector2i(x * iv, y * iv);}

	inline int LengthSqr() const						{return x * x + y * y;}
	inline float Length() const							{return VSqrt(x * x + y * y);}
};

class VVector3i
{

};

class VVector4i
{

};

//////////////////////////////////////////////////////////////////////////
// VPlane
//////////////////////////////////////////////////////////////////////////

class VPlane
{
public:
	float x,y,z,w;

	inline VPlane(){}
	inline VPlane(float _x, float _y, float _z, float _w):x(_x),y(_y),z(_z),w(_w){}
	inline VPlane(const VPlane& p):x(p.x),y(p.y),z(p.z),w(p.w){}

	inline float Distance(const VVector3& Point) const 
	{
		return x*Point.x + y*Point.y + z*Point.z + w;
	}


};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
VALIGNED_PREFIX(16) class VQuat
{
public:
	static VCoreApi const VQuat	IDENTITY;
	float x,y,z,w;
	inline VQuat(){}
	inline VQuat(float X, float Y, float Z, float W):x(X),y(Y),z(Z),w(W){}
	inline VQuat(const VVector3& Axis, float Angle)		{ FromAxisAngle(Axis, Angle); }
	explicit inline VQuat(const VMatrix4X4& Mat)		{ FromRotateMatrix(Mat); }
	explicit inline VQuat(const VVector3& EulerAngle)	{ FromEulerAngle(EulerAngle);}
	explicit inline VQuat(const VVector4& Vec4);

	// 共轭矩阵.(逆)
	inline VQuat operator-() const							{ return VQuat(-x, -y, -z, w); }
	inline VQuat operator+( const VQuat& q ) const			{ return VQuat( x + q.x, y + q.y, z + q.z, w + q.w );}
	inline void operator+=( const VQuat& q ) 				{ x+=q.x; y+=q.y; z+=q.z; w+=q.w; }
	inline VQuat operator-( const VQuat& q ) const			{ return VQuat( x - q.x, y - q.y, z - q.z, w - q.w );}
	inline void operator-=( const VQuat& q ) 				{ x-=q.x; y-=q.y; z-=q.z; w-=q.w; }
	// 如果需要判断近似相等,使用VEqual(q1,q2,eps);
	BOOL operator==( const VQuat& q ) const					{ return x==q.x && y==q.y && z==q.z &&  w==q.w;}
	BOOL operator!=( const VQuat& q ) const					{ return x!=q.x || y!=q.y || z!=q.z || w!=q.w;}
	// Note: 组合原则是Q' = Q2*Q1
	inline VQuat operator*(const VQuat& q) const;

	inline BOOL IsNormalized() const 
	{
		float sqr = VSQR(x)+VSQR(y)+VSQR(z)+VSQR(w);
		return VAbs(1.0f - sqr) < 0.01f;
	}
	inline void Normalize();
	inline void Identity();
	inline float Dot(const VQuat& q) const					{return x*q.x + y*q.y + z*q.z + w*q.w;}

	//template<class MatrixType>
	//inline void FromRotateMatrix(const MatrixType& RotateMat);
	inline void FromRotateMatrix(const VMatrix4X4& RotateMat);
	// from axis angle. 确保axis 单位化.
	inline void FromAxisAngle(const VVector3& Axis,float Angle);

	// init from eluer angles (angle in degrees)
	inline void FromEulerAngle(const VVector3& EulerAngle)					{ FromEulerAngle(EulerAngle.x, EulerAngle.y, EulerAngle.z); }
	inline void FromEulerAngle(float XAngle, float YAngle, float ZAngle);
	// init from tow axis , rotate axis from ->  axis to.
	inline void FromTwoAxis(const VVector3& from, const VVector3& to);

	inline void ToRotateMatrix(VMatrix4X4& RotateMat) const;
	inline void ToAxisAngle(VVector3& Axis, float& Angle) const;
	inline void ToEulerAngle(VVector3& EulerAngle) const					{ ToEulerAngle(EulerAngle.x, EulerAngle.y, EulerAngle.z);}
	inline void ToEulerAngle(float& XAngle, float& YAngle, float& ZAngle) const;

	// 旋转
	inline VVector3 Rotate(const VVector3& Vec) const;
	inline VVector4 Rotate(const VVector4& Vec) const;
}VALIGNED_SUFFIX(16);



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
class VMatrix4X4
{
public:
	VALIGNED_PREFIX(16) union
	{
		float m[4][4];
		struct {
			float m11,m12,m13,m14;
			float m21,m22,m23,m24;
			float m31,m32,m33,m34;
			float m41,m42,m43,m44;
		};
#ifdef V_SSE
		struct {
			__m128 row0;
			__m128 row1;
			__m128 row2;
			__m128 row3;
		};
#elif V_NEON
		struct {
			float32x4_t row0;
			float32x4_t row1;
			float32x4_t row2;
			float32x4_t row3;
		};
#endif
	}VALIGNED_SUFFIX(16);
	inline VMatrix4X4(){}
	inline VMatrix4X4(float _11,float _12, float _13, float _14,
		float _21,float _22, float _23, float _24,
		float _31,float _32, float _33, float _34,
		float _41,float _42, float _43, float _44
		)
		:m11(_11),m12(_12),m13(_13),m14(_14)
		,m21(_21),m22(_22),m23(_23),m24(_24)
		,m31(_31),m32(_32),m33(_33),m34(_34)
		,m41(_41),m42(_42),m43(_43),m44(_44)
	{}



	//////////////////////////////////////////////////////////////////////////
	// operator
	//////////////////////////////////////////////////////////////////////////
	inline VMatrix4X4	operator*(float s) const;
	inline void		operator*=(float s);
	inline VMatrix4X4	operator*(const VMatrix4X4& rm) const;
	inline void		operator*=(const VMatrix4X4& rm);
	inline VMatrix4X4	operator+(const VMatrix4X4& rm) const;
	inline void		operator+=(const VMatrix4X4& rm);

	inline void SetIdentity();
	inline float Determinant() const;
	inline float Determinant33() const;
	inline VMatrix4X4 Transpose() const;
	inline VMatrix4X4 Inverse() const;

	inline VVector3 GetRow(int RowIndex) const {	return VVector3(m[RowIndex][0], m[RowIndex][1],m[RowIndex][2]); }
	// 设置为平移矩阵
	inline void SetTranslate(float tx,float ty, float tz);
	inline void SetTranslate(const VVector3& Trans) { SetTranslate(Trans.x, Trans.y, Trans.z); }
	inline void SetScale(float x, float y, float z);
	inline void SetScale(const VVector3& Scale)		{ SetScale(Scale.x, Scale.y, Scale.z); }
	// 设置为绕指定轴旋转的旋转矩阵
	inline void SetRotateFromAxisAngle(const VVector3& Axis, float Angle);
	// 构建旋转矩阵, 将矢量From 旋转到 矢量To. 
	inline bool SetRotateFromTwoVectors(const VVector3& From, const VVector3& To);
	// 设置为指定Quaternion的旋转矩阵
	inline void SetRotateFromQuat(const VQuat& Quat);
	inline void SetRotateX(float Angle);
	inline void SetRotateY(float Angle);
	inline void SetRotateZ(float Angle);
	// 从矩阵中移除缩放成分
	inline void RemoveScale();
	inline VVector3 GetScale() const;
	inline void TransformVec3(VVector3& Out, const VVector3& V) const;

	// will projecting the w = 1.
	inline void TransformVec3Projection(VVector3& Out, const VVector3& V) const
	{
		VVector4 Projected(V, 1.0f);
		TransformVec4(Projected, Projected);
		float InvW = 1.0f / Projected.w;
		Out.x = Projected.x * InvW;Out.y = Projected.y * InvW; Out.z = Projected.z * InvW;
	}
	inline void TransformVec4(VVector4& Out, const VVector4& V) const;
	//将点V逆变换到当前空间(仅仅适用于没有任何缩放的正交变换矩阵, 对于有缩放的矩阵, 请自行Inv(M)后变换.)
	inline void InvTransformVec3NoScale(VVector3& Out, const VVector3& V) const;

	// 变换方向矢量
	inline void TransformNormal(VVector3& Out, const VVector3& V) const;
	inline void TransformNormal(VByteVector& Out, const VByteVector& V) const;
	//将点V逆变换到当前空间(仅仅适用于没有任何缩放的正交变换矩阵, 对于有缩放的矩阵, 请自行Inv(M)后变换.)
	inline void InvTransformNormalNoScale(VVector3& Out, const VVector3& V) const;

	// set matrix from basis axis and basis orgin, will convert to the basis local space.
	inline void SetFromBasis(const VVector3& Origin, const VVector3& XAxis, const VVector3& YAxis, const VVector3& ZAxis)
	{
		m11 = XAxis.x; m12 = YAxis.x; m13 = ZAxis.x; m14 = 0.0f;
		m21 = XAxis.y; m22 = YAxis.y; m23 = ZAxis.y; m24 = 0.0f;
		m31 = XAxis.z; m32 = YAxis.z; m33 = ZAxis.z; m34 = 0.0f;
		m41 = -Origin.Dot(XAxis); 
		m42 = -Origin.Dot(YAxis); 
		m43 = -Origin.Dot(ZAxis); m44 = 1.0f;
	}

	// 初始化为ViewMatrix.
	// Right Hand
	inline void LookAt(const VVector3& Eye, const VVector3& At, const VVector3& Up);

	// 初始化为投影矩阵 (Right Hand)
	// Fov : field of view in YOZ plane.(degree)
	// Aspect : view space width / height.
	inline void Perspective(float Fov, float Aspect, float Near, float Far);
	inline void Frustum();
	inline void OrthoOffCenter(float Left, float Right, float Bottom, float Top, float Near, float Far);

	inline void Reflect(const VVector4& Plane);

	// Decompose matrix to scale,rot translate.
	// matrix transform compose as : S * R * T  
	VCoreApi void Decompose(VVector3& Scale, VQuat& Rot, VVector3& Transltate) const;
	VCoreApi void DecomposeFast(VVector3& Scale, VQuat& Rot, VVector3& Transltate) const;
};
typedef VMatrix4X4 VMatrix;	


inline void VMatrixLookAt(VMatrix& Matrix, const VVector3& Eye, const VVector3& At, const VVector3& Up)
{
	Matrix.LookAt(Eye, At, Up);
}

// constants
extern VCoreApi const VVector3 VVec3_0;
extern VCoreApi const VVector3 VVec3_1;
extern VCoreApi const VVector3 VVec3_X;
extern VCoreApi const VVector3 VVec3_Y;
extern VCoreApi const VVector3 VVec3_Z;
extern VCoreApi const VVector3 VVec3_EPS;
extern VCoreApi const VVector3 VVec3_INF;



inline float VLerp(float v0,float v1,float k) 
{
	return v0 + (v1 - v0) * k;
}







// global operator

inline VVector3 operator*(float s, const VVector3 &V)
{
	return V * s;
}

inline VVector3 operator*(const VVector3 &V,const VMatrix4X4 &M)
{
	VVector3 Ret;
	M.TransformVec3(Ret, V);
	return Ret;
}

inline VVector4 operator*(const VVector4 &V,const VMatrix4X4 &M)
{
	VVector4 Ret;
	M.TransformVec4(Ret, V);
	return Ret;
}

//////////////////////////////////////////////////////////////////////////
//	inline implemention
//////////////////////////////////////////////////////////////////////////
inline BOOL VEqual(float f0, float f1, float Eps)
{
	return VAbs(f0 - f1) < Eps; 
}
inline BOOL VEqual(const VVector3& v0, const VVector3& v1, float Eps)
{
	return VEqual(v0.x, v1.x, Eps) && VEqual(v0.y, v1.y, Eps) && VEqual(v0.z, v1.z, Eps);
}
inline BOOL VEqual(const VVector4& v0, const VVector4& v1, float Eps)
{
	return VEqual(v0.x, v1.x, Eps) && VEqual(v0.y, v1.y, Eps) && VEqual(v0.z, v1.z, Eps) && VEqual(v0.w,v1.w, Eps);
}
inline BOOL VEqual(const VQuat& q0, const VQuat& q1, float Eps)
{
	return VEqual(q0.x, q1.x, Eps) && VEqual(q0.y, q1.y, Eps) && VEqual(q0.z, q1.z, Eps) && VEqual(q0.w, q1.w, Eps);
}


//////////////////////////////////////////////////////////////////////////
// Vector
//////////////////////////////////////////////////////////////////////////
inline VVector3::VVector3(const VVector4 &v):x(v.x), y(v.y), z(v.z)
{
}

//////////////////////////////////////////////////////////////////////////
// Quaternion
//////////////////////////////////////////////////////////////////////////
inline VQuat VQuat::operator*(const VQuat& q) const
{
#if 0//V_SSE
	/*  working code !!!!
	__m128 temp =  _MM_SWIZZLE(vec,W,W,W,W);	// [w,w,w,w]
	__m128 result = _mm_mul_ps(temp, q.vec);	// [w*q.x, w*q.y, w*q.z, w*q.w]
	temp = _MM_SWIZZLE(q.vec,W,Z,Y,X);			// temp = [q.w,q.z,q.y,q.x]
	__m128 temp1 = _MM_SWIZZLE(vec,X,X,X,X);	// temp1 = [x,x,x,x];
	temp = _mm_mul_ps(temp1, temp);				// temp = [q.w*x , q.z*x, q.y*x, q.x*x]
	temp = _mm_mul_ps(temp, VSSE_CR1_11_1);		// temp = [q.w*x, -q.z*x, q.y*x, -q.x*x];
	result = _mm_add_ps(temp,result);			// result = [w*q.x + q.w*x , w*q.y-q.z*x, w*q.z + q.y*x, w*q.w - q.x*x]

	temp = _MM_SWIZZLE(q.vec,Z,W,X,Y);			// temp = [q.z,q.w,q.x,q.y]
	temp1 = _MM_SWIZZLE(vec,Y,Y,Y,Y);			// temp1 = [y,y,y,y];
	temp = _mm_mul_ps(temp1, temp);				// temp = [q.z*y, q.w*y, q.x*y, q.y*y]
	temp = _mm_mul_ps(temp, VSSE_CR11_1_1);		// temp = [q.z*y, q.w*y, -q.x*y, -q.y*y]
	result = _mm_add_ps(temp,result);			// result = [w*q.x + q.w*x +q.z*y, w*q.y-q.z*x + q.w*y, w*q.z + q.y*x - q.x*y, w*q.w - q.x*x - q.y*y]

	temp = _MM_SWIZZLE(q.vec,Y,X,W,Z);			// temp = [q.y,q.x,q.w,q.z]
	temp1 = _MM_SWIZZLE(vec,Z,Z,Z,Z);			// temp1 = [z,z,z,z];
	temp = _mm_mul_ps(temp1, temp);				// temp = [q.y*z,q.x*z,q.w*z,q.z*z]
	temp = _mm_mul_ps(temp, VSSE_CR_111_1);		// temp = [-q.y*z,q.x*z,q.w*z,-q.z*z]
	result = _mm_add_ps(temp,result);			// result = [w*q.x + q.w*x +q.z*y -q.y*z, w*q.y-q.z*x + q.w*y + q.x*z, w*q.z + q.y*x - q.x*y, w*q.w - q.x*x - q.y*y]
	*/
	VQuat Ret;
	Ret.vec = _mm_mul_ps(_MM_SWIZZLE(vec,W,W,W,W), q.vec);						// [w*q.x, w*q.y, w*q.z, w*q.w]
	Ret.vec = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(_MM_SWIZZLE(vec,X,X,X,X), _MM_SWIZZLE(q.vec,W,Z,Y,X)), VSSE_CR1_11_1),Ret.vec);	// result = [w*q.x + q.w*x , w*q.y-q.z*x, w*q.z + q.y*x, w*q.w - q.x*x]
	Ret.vec = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(_MM_SWIZZLE(vec,Y,Y,Y,Y), _MM_SWIZZLE(q.vec,Z,W,X,Y)), VSSE_CR11_1_1),Ret.vec);	// result = [w*q.x + q.w*x +q.z*y, w*q.y-q.z*x + q.w*y, w*q.z + q.y*x - q.x*y, w*q.w - q.x*x - q.y*y]
	Ret.vec = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(_MM_SWIZZLE(vec,Z,Z,Z,Z), _MM_SWIZZLE(q.vec,Y,X,W,Z)), VSSE_CR_111_1),Ret.vec);	//result = [w*q.x + q.w*x +q.z*y -q.y*z, w*q.y-q.z*x + q.w*y + q.x*z, w*q.z + q.y*x - q.x*y+q.w*z, w*q.w - q.x*x - q.y*y-q.z*z]
	return Ret;

#else
	return VQuat(w*q.x + q.w*x + y*q.z - q.y*z, w*q.y + q.w*y + z*q.x - q.z*x, w*q.z + q.w*z + x*q.y - q.x*y, w*q.w - x*q.x - y*q.y - z*q.z);
#endif 
}

inline void VQuat::Normalize()
{
	float sqr = VSQR(x)+VSQR(y)+VSQR(z)+VSQR(w);
	if (sqr >= VEPS)
	{
		float Scale = VInvSqrt(sqr);
		x *= Scale; y *= Scale; z *= Scale; w*=Scale;
	}else
	{
		Identity();
	}
}

inline void VQuat::Identity()
{
	x=y=z=0.0f;w=1.0f;
}

//template<class MatrixType>
inline void VQuat::FromRotateMatrix(const VMatrix4X4& RotateMat)
{
	float fTrace, s;
	static const int nxt[3] = { 1, 2, 0 };
	fTrace = RotateMat.m11 + RotateMat.m22 + RotateMat.m33;

	if (fTrace > 0.0f) 
	{
		s = VSqrt(fTrace + 1.0f);
		w = s * 0.5f;
		s = 0.5f / s;
		x = (RotateMat.m23 - RotateMat.m32) * s;
		y = (RotateMat.m31 - RotateMat.m13) * s;
		z = (RotateMat.m12 - RotateMat.m21) * s;
	} 
	else 
	{
		// diagonal is negative
		int i, j, k;
		float qt[4];
		i = 0;
		if (RotateMat.m22 > RotateMat.m11)
			i = 1;
		if (RotateMat.m33 > RotateMat.m[i][i])
			i = 2;
		j = nxt[i];
		k = nxt[j];

		s = RotateMat.m[i][i] - RotateMat.m[j][j] - RotateMat.m[k][k] + 1.0f;
		s = VSqrt(s);
		qt[i] = s * 0.5f;
		if (s != 0.0f)
			s = 0.5f / s;
		qt[3] = (RotateMat.m[j][k] - RotateMat.m[k][j]) * s;
		qt[j] = (RotateMat.m[i][j] + RotateMat.m[j][i]) * s;
		qt[k] = (RotateMat.m[i][k] + RotateMat.m[k][i]) * s;
		x = qt[0];
		y = qt[1];
		z = qt[2];
		w = qt[3];
	}
}

inline void VQuat::FromAxisAngle(const VVector3& Axis,float Angle)
{
	VASSERT(Axis.IsNomalized());
	Angle = Angle * 0.5f;
	float S,C;
	VSinCos(Angle, S, C);
	x = S * Axis.x;
	y = S * Axis.y;
	z = S * Axis.z;
	w = C;
}

inline void VQuat::FromEulerAngle(float XAngle, float YAngle, float ZAngle)
{
	float cx, sx,cy,sy,cz, sz;
	VSinCos( VDEG_RAD(XAngle) * 0.5f, sx, cx );
	VSinCos( VDEG_RAD(YAngle) * 0.5f, sy, cy );
	VSinCos( VDEG_RAD(ZAngle) * 0.5f, sz, cz );
	float cycz, sysz, sycz, cysz;
	cycz = cy*cz;
	sysz = sy*sz;
	sycz = sy*cz;
	cysz = cy*sz;
	x = cycz*sx + sysz*cx;
	y = sycz*cx - cysz*sx;
	z = cysz*cx - sycz*sx;
	w = cycz*cx + sysz*sx;
}

inline void VQuat::FromTwoAxis(const VVector3& From, const VVector3& To)
{
	VVector3 vCross = From.Cross(To);
	float crossLen = vCross.Length();
	if(crossLen < VEPS)
	{
		//平行.
		Identity();	
		return;
	}
	float angle = VASin(crossLen);
	float dot = From.Dot(To);
	if(dot < 0.0f)
		angle = VPI - angle;
	float sinHalfAng,cosHalfAng;
	VSinCos(0.5f * angle, sinHalfAng, cosHalfAng);
	vCross *= 1.0f /crossLen;		// 单位化
	x = sinHalfAng * vCross.x;
	y = sinHalfAng * vCross.y;
	z = sinHalfAng * vCross.z;
	w = cosHalfAng;
}
inline void VQuat::ToRotateMatrix(VMatrix4X4& RotateMat) const
{
	// NOTE: 我使用行矩阵. 这里的转换和大多数教材上的是一个倒置关系.
	FLOAT wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	x2 = x * 2.0f;  y2 = y * 2.0f;  z2 = z *2.0f;
	xx = x* x2;   xy = x * y2;   xz =x * z2;
	yy = y * y2;   yz = y * z2;   zz = z * z2;
	wx = w * x2;   wy = w * y2;   wz = w * z2;

	RotateMat.m11 = 1.0f - (yy + zz);
	RotateMat.m21 = xy - wz;
	RotateMat.m31 = xz + wy;
	RotateMat.m41 = 0.0f;

	RotateMat.m12 = xy + wz;
	RotateMat.m22 = 1.0f - (xx + zz);
	RotateMat.m32 = yz - wx;
	RotateMat.m42 = 0.0f;

	RotateMat.m13 = xz - wy;
	RotateMat.m23 = yz + wx;
	RotateMat.m33 = 1.0f - (xx + yy);
	RotateMat.m43 = 0.0f;

	RotateMat.m14 = 0.0f;
	RotateMat.m24 = 0.0f;
	RotateMat.m34 = 0.0f;
	RotateMat.m44 = 1.0f;
}

inline void VQuat::ToAxisAngle(VVector3& Axis, float& Angle) const
{
	float fSqrLength = x*x+y*y+z*z;
	if ( fSqrLength > 0.0 )
	{
		Angle = 2.0f*VCos(w);
		float fInvLength = VInvSqrt(fSqrLength);
		Axis.x = x*fInvLength;
		Axis.y = y*fInvLength;
		Axis.z = z*fInvLength;
	}
	else
	{
		Angle = 0.0f;
		Axis.x = 1.0;
		Axis.y = 0.0;
		Axis.z = 0.0;
	}
}
inline void VQuat::ToEulerAngle(float& XAngle, float& YAngle, float& ZAngle) const
{
	//1. 转换为旋转矩阵.
	//2. DECOMP 矩阵,取出欧拉角. ref: http://www.darwin3d.com/gamedev/quat2eul.cpp
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
	float sinx = (w*x - y*z)*2.0f;
	if (VAbs(sinx) > 0.9999f)
	{
		XAngle = VRAD_DEG(VHALF_PI * sinx);
		YAngle = VRAD_DEG(atan2f(w*y - x*z , 0.5f - y*y - z*z));
		ZAngle = 0.0f;
	}else
	{
		float sqx = x * x;
		YAngle = VRAD_DEG(atan2f(x*z + y*w , 0.5f - sqx - y*y));
		XAngle = VRAD_DEG(asinf(sinx));
		ZAngle = VRAD_DEG(atan2f(x*y + z*w , 0.5f - sqx - z*z));
	}
}

inline VVector3 VQuat::Rotate(const VVector3& Vec) const
{
	//From nVidia SDK 
	VVector3 uv, uuv;
	VVector3 qvec(x, y, z);
	uv = qvec.Cross(Vec);			// 6* 3-
	uuv = qvec.Cross(uv);			// 6* 3-
	uv *= (2.0f * w);				// 2*  
	uuv *= 2.0f;					// * 
	return Vec + uv + uuv;			// 2+				15*  6- 2+ 

	//float xx=x*x;
	//float zz=z*z;
	//float yy=y*y; 
	//float xy=x*y;
	//float xz=x*z;
	//float yz=y*z; 
	//float wx=w*x,wy=w*y,wz=w*z;
	//
	//float resx = v.x*(1-(yy+zz)*2)	+ v.y*(xy+wz)*2		+ v.z*(xz-wy)*2;
	//float resy = v.x*(xy-wz)*2		+ v.y*(1-(xx+zz)*2)	+ v.z*(yz+wx)*2;
	//float resz = v.x*(xz+wy)*2		+ v.y*(yz-wx)*2		+ v.z*(1-(xx+yy)*2);
	//return LVector(resx,resy,resz);
}

inline VVector4 VQuat::Rotate(const VVector4& Vec) const
{
	VVector3 Vec3(Vec);
	Vec3 = Rotate(Vec3);
	return VVector4(Vec3, Vec.w);
}
//////////////////////////////////////////////////////////////////////////
// Matrix
//////////////////////////////////////////////////////////////////////////
inline VMatrix4X4 VMatrix4X4::operator*(float s) const
{
	VMatrix4X4 ret(*this);
	ret *= s;
	return ret;
}

inline void	VMatrix4X4::operator*=(float s)
{
#if V_SSE
	__m128 temp = _mm_set1_ps(s);
	row0 = _mm_mul_ps(row0,temp);
	row1 = _mm_mul_ps(row1,temp);
	row2 = _mm_mul_ps(row2,temp);
	row3 = _mm_mul_ps(row3,temp);
#elif V_NEON
	row0 = vmulq_n_f32(row0,s);
	row1 = vmulq_n_f32(row1,s);
	row2 = vmulq_n_f32(row2,s);
	row3 = vmulq_n_f32(row3,s);
#else
	m11*=s;	m12*=s;	m13*=s; m14*=s;
	m21*=s;	m22*=s;	m23*=s; m24*=s;
	m31*=s;	m32*=s;	m33*=s; m34*=s;
	m41*=s;	m42*=s;	m43*=s; m44*=s;
#endif
}

inline VMatrix4X4 VMatrix4X4::operator*(const VMatrix4X4& rm) const
{
	VMatrix4X4 Ret;
#ifdef V_SSE
	__m128 temp = _MM_SWIZZLE(row0,X,X,X,X);
	temp = _mm_mul_ps(temp, rm.row0);

	temp = _MM_SWIZZLE(row0,W,W,W,W);
	temp = _mm_mul_ps(temp, rm.row0);

	__m128 res_0 = _mm_mul_ps(_MM_SWIZZLE(row0,X,X,X,X), rm.row0);		// m11 * [m11 m12 m13 m14]'
	__m128 res_1 = _mm_mul_ps(_MM_SWIZZLE(row0,Y,Y,Y,Y), rm.row1);		// m12 * [m21 m22 m23 m24]'
	__m128 res_2 = _mm_mul_ps(_MM_SWIZZLE(row0,Z,Z,Z,Z), rm.row2);		// m13 * [m31 m32 m33 m34]'
	__m128 res_3 = _mm_mul_ps(_MM_SWIZZLE(row0,W,W,W,W), rm.row3);		// m14 * [m41 m42 m43 m44]'
	Ret.row0 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,res_3));
	res_0 = _mm_mul_ps(_MM_SWIZZLE(row1,X,X,X,X), rm.row0);
	res_1 = _mm_mul_ps(_MM_SWIZZLE(row1,Y,Y,Y,Y), rm.row1);
	res_2 = _mm_mul_ps(_MM_SWIZZLE(row1,Z,Z,Z,Z), rm.row2);	
	res_3 = _mm_mul_ps(_MM_SWIZZLE(row1,W,W,W,W), rm.row3);
	Ret.row1 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,res_3));
	res_0 = _mm_mul_ps(_MM_SWIZZLE(row2,X,X,X,X), rm.row0);
	res_1 = _mm_mul_ps(_MM_SWIZZLE(row2,Y,Y,Y,Y), rm.row1);
	res_2 = _mm_mul_ps(_MM_SWIZZLE(row2,Z,Z,Z,Z), rm.row2);	
	res_3 = _mm_mul_ps(_MM_SWIZZLE(row2,W,W,W,W), rm.row3);
	Ret.row2 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,res_3));
	res_0 = _mm_mul_ps(_MM_SWIZZLE(row3,X,X,X,X), rm.row0);
	res_1 = _mm_mul_ps(_MM_SWIZZLE(row3,Y,Y,Y,Y), rm.row1);
	res_2 = _mm_mul_ps(_MM_SWIZZLE(row3,Z,Z,Z,Z), rm.row2);	
	res_3 = _mm_mul_ps(_MM_SWIZZLE(row3,W,W,W,W), rm.row3);
	Ret.row3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,res_3));
#elif USE_NEON
	float32x2_t t0 = vget_low_f32(row0);
	float32x2_t t1 = vget_high_f32(row0);
	float32x4_t temp = vmulq_lane_f32(rm.row0, t0, 0);
	temp = vmlaq_lane_f32(temp, rm.row1, t0, 1 );		// WTF!!! neon!! mla(a,b,c) -> ret = a + b * c !!!! not a * b + c!!!!! 
	temp = vmlaq_lane_f32(temp, rm.row2, t1, 0 );
	Ret.row0 = vmlaq_lane_f32(temp, rm.row3, t1, 1 );
	t0 = vget_low_f32(row1);
	t1 = vget_high_f32(row1);
	temp = vmulq_lane_f32(rm.row0, t0, 0);
	temp = vmlaq_lane_f32(temp, rm.row1, t0, 1 );
	temp = vmlaq_lane_f32(temp, rm.row2, t1, 0 );
	Ret.row1 = vmlaq_lane_f32(temp, rm.row3, t1, 1 );
	t0 = vget_low_f32(row2);
	t1 = vget_high_f32(row2);
	temp = vmulq_lane_f32(rm.row0, t0, 0);
	temp = vmlaq_lane_f32(temp, rm.row1, t0, 1 );
	temp = vmlaq_lane_f32(temp, rm.row2, t1, 0 );
	Ret.row2 = vmlaq_lane_f32(temp, rm.row3, t1, 1 );
	t0 = vget_low_f32(row3);
	t1 = vget_high_f32(row3);
	temp = vmulq_lane_f32(rm.row0, t0, 0);
	temp = vmlaq_lane_f32(temp, rm.row1, t0, 1 );
	temp = vmlaq_lane_f32(temp, rm.row2, t1, 0 );
	Ret.row3 = vmlaq_lane_f32(temp, rm.row3, t1, 1 );
#else
	Ret.m11 = m11 * rm.m11 + m12 * rm.m21 + m13 * rm.m31 + m14 * rm.m41;
	Ret.m12 = m11 * rm.m12 + m12 * rm.m22 + m13 * rm.m32 + m14 * rm.m42;
	Ret.m13 = m11 * rm.m13 + m12 * rm.m23 + m13 * rm.m33 + m14 * rm.m43;
	Ret.m14 = m11 * rm.m14 + m12 * rm.m24 + m13 * rm.m34 + m14 * rm.m44;
	Ret.m21 = m21 * rm.m11 + m22 * rm.m21 + m23 * rm.m31 + m24 * rm.m41;
	Ret.m22 = m21 * rm.m12 + m22 * rm.m22 + m23 * rm.m32 + m24 * rm.m42;
	Ret.m23 = m21 * rm.m13 + m22 * rm.m23 + m23 * rm.m33 + m24 * rm.m43;
	Ret.m24 = m21 * rm.m14 + m22 * rm.m24 + m23 * rm.m34 + m24 * rm.m44;
	Ret.m31 = m31 * rm.m11 + m32 * rm.m21 + m33 * rm.m31 + m34 * rm.m41;
	Ret.m32 = m31 * rm.m12 + m32 * rm.m22 + m33 * rm.m32 + m34 * rm.m42;
	Ret.m33 = m31 * rm.m13 + m32 * rm.m23 + m33 * rm.m33 + m34 * rm.m43;
	Ret.m34 = m31 * rm.m14 + m32 * rm.m24 + m33 * rm.m34 + m34 * rm.m44;
	Ret.m41 = m41 * rm.m11 + m42 * rm.m21 + m43 * rm.m31 + m44 * rm.m41;
	Ret.m42 = m41 * rm.m12 + m42 * rm.m22 + m43 * rm.m32 + m44 * rm.m42;
	Ret.m43 = m41 * rm.m13 + m42 * rm.m23 + m43 * rm.m33 + m44 * rm.m43;
	Ret.m44 = m41 * rm.m14 + m42 * rm.m24 + m43 * rm.m34 + m44 * rm.m44;
#endif
	return Ret;
}
inline void	VMatrix4X4::operator*=(const VMatrix4X4& rm)
{
	*this = *this * rm;
}
inline VMatrix4X4 VMatrix4X4::operator+(const VMatrix4X4& rm) const
{
	VMatrix4X4 ret(*this);
	ret += rm;
	return ret;
}
inline void	VMatrix4X4::operator+=(const VMatrix4X4& rm)
{
#ifdef V_SSE
	row0 = _mm_add_ps(row0,rm.row0);
	row1 = _mm_add_ps(row1,rm.row1);
	row2 = _mm_add_ps(row2,rm.row2);
	row3 = _mm_add_ps(row3,rm.row3);
#elif V_NEON
	row0 = vaddq_f32(row0,rm.row0);
	row1 = vaddq_f32(row1,rm.row1);
	row2 = vaddq_f32(row2,rm.row2);
	row3 = vaddq_f32(row3,rm.row3);
#else
	m11 += rm.m11; m12 += rm.m12; m13 += rm.m13; m14 += rm.m14;
	m21 += rm.m21; m22 += rm.m22; m23 += rm.m23; m24 += rm.m24;
	m31 += rm.m31; m32 += rm.m32; m33 += rm.m33; m34 += rm.m34;
	m41 += rm.m41; m42 += rm.m42; m43 += rm.m43; m44 += rm.m44;
#endif
}

inline void VMatrix4X4::SetIdentity()
{
	m11 = 1.0f; m12 = 0.0f;  m13 = 0.0f;  m14 = 0.f;
	m21 = 0.0f; m22 = 1.0f;  m23 = 0.0f;  m24 = 0.f;
	m31 = 0.0f; m32 = 0.0f;  m33 = 1.0f;  m34 = 0.0f;
	m41 = 0.0f; m42 = 0.0f;  m43 = 0.0f;  m44 = 1.0f;
}

inline float VMatrix4X4::Determinant() const 
{
	return	m11 * (
		m22 * (m33 * m44 - m34 * m43) -
		m32 * (m23 * m44 - m24 * m43) +
		m42 * (m23 * m34 - m24 * m33)
		) -
		m21 * (
		m12 * (m33 * m44 - m34 * m43) -
		m32 * (m13 * m44 - m14 * m43) +
		m42 * (m13 * m34 - m14 * m33)
		) +
		m31 * (
		m12 * (m23 * m44 - m24 * m43) -
		m22 * (m13 * m44 - m14 * m43) +
		m42 * (m13 * m24 - m14 * m23)
		) -
		m41 * (
		m12 * (m23 * m34 - m24 * m33) -
		m22 * (m13 * m34 - m14 * m33) +
		m32 * (m13 * m24 - m14 * m23)
		);
}

inline float VMatrix4X4::Determinant33() const
{
	return	m11 * (m22 * m33 - m23 * m32) -
		m21 * (m12 * m33 - m13 * m32) +
		m31 * (m12 * m23 - m13 * m22);
}

inline VMatrix4X4 VMatrix4X4::Transpose() const
{
	// TODO SSE UNIGINE.
	VMatrix4X4	ret;
	ret.m11 = m11;ret.m12 = m21;ret.m13 = m31;ret.m14 = m41;
	ret.m21 = m12;ret.m22 = m22;ret.m23 = m32;ret.m24 = m42;
	ret.m31 = m13;ret.m32 = m23;ret.m33 = m33;ret.m34 = m43;
	ret.m41 = m14;ret.m42 = m24;ret.m43 = m34;ret.m44 = m44;
	return ret;
}

inline VMatrix4X4 VMatrix4X4::Inverse() const
{
#ifdef V_SSE
	VMatrix4X4 InvMat;
	__m128 res_0 = _mm_shuffle_ps(row0,row1,VSHUFFLE_MASK(X,Y,X,Y));
	__m128 res_1 = _mm_shuffle_ps(row0,row1,VSHUFFLE_MASK(Z,W,Z,W));
	__m128 res_2 = _mm_shuffle_ps(row2,row3,VSHUFFLE_MASK(X,Y,X,Y));
	__m128 res_3 = _mm_shuffle_ps(row2,row3,VSHUFFLE_MASK(Z,W,Z,W));
	__m128 row_0 = _mm_shuffle_ps(res_0,res_2,VSHUFFLE_MASK(X,Z,X,Z));
	__m128 row_1 = _mm_shuffle_ps(res_2,res_0,VSHUFFLE_MASK(Y,W,Y,W));
	__m128 row_2 = _mm_shuffle_ps(res_1,res_3,VSHUFFLE_MASK(X,Z,X,Z));
	__m128 row_3 = _mm_shuffle_ps(res_3,res_1,VSHUFFLE_MASK(Y,W,Y,W));
	__m128 temp = _mm_mul_ps(row_2,row_3);
	temp = _MM_SWIZZLE(temp,Y,X,W,Z);
	res_0 = _mm_mul_ps(row_1,temp);
	res_1 = _mm_mul_ps(row_0,temp);
	temp = _MM_SWIZZLE(temp,Z,W,X,Y);
	res_0 = _mm_sub_ps(_mm_mul_ps(row_1,temp),res_0);
	res_1 = _mm_sub_ps(_mm_mul_ps(row_0,temp),res_1);
	res_1 = _MM_SWIZZLE(res_1,Z,W,X,Y);
	temp = _mm_mul_ps(row_1,row_2);
	temp = _MM_SWIZZLE(temp,Y,X,W,Z);
	res_0 = _mm_add_ps(_mm_mul_ps(row_3,temp),res_0);
	res_3 = _mm_mul_ps(row_0,temp);
	temp = _MM_SWIZZLE(temp,Z,W,X,Y);
	res_0 = _mm_sub_ps(res_0,_mm_mul_ps(row_3,temp));
	res_3 = _mm_sub_ps(_mm_mul_ps(row_0,temp),res_3);
	res_3 = _MM_SWIZZLE(res_3,Z,W,X,Y);
	temp = _mm_mul_ps(row_3,_MM_SWIZZLE(row_1,Z,W,X,Y));
	temp = _MM_SWIZZLE(temp,Y,X,W,Z);
	row_2 = _MM_SWIZZLE(row_2,Z,W,X,Y);
	res_0 = _mm_add_ps(_mm_mul_ps(row_2,temp),res_0);
	res_2 = _mm_mul_ps(row_0,temp);
	temp = _MM_SWIZZLE(temp,Z,W,X,Y);
	res_0 = _mm_sub_ps(res_0,_mm_mul_ps(row_2,temp));
	res_2 = _mm_sub_ps(_mm_mul_ps(row_0,temp),res_2);
	res_2 = _MM_SWIZZLE(res_2,Z,W,X,Y);
	temp = _mm_mul_ps(row_0,row_1);
	temp = _MM_SWIZZLE(temp,Y,X,W,Z);
	res_2 = _mm_add_ps(_mm_mul_ps(row_3,temp),res_2);
	res_3 = _mm_sub_ps(_mm_mul_ps(row_2,temp),res_3);
	temp = _MM_SWIZZLE(temp,Z,W,X,Y);
	res_2 = _mm_sub_ps(_mm_mul_ps(row_3,temp),res_2);
	res_3 = _mm_sub_ps(res_3,_mm_mul_ps(row_2,temp));
	temp = _mm_mul_ps(row_0,row_3);
	temp = _MM_SWIZZLE(temp,Y,X,W,Z);
	res_1 = _mm_sub_ps(res_1,_mm_mul_ps(row_2,temp));
	res_2 = _mm_add_ps(_mm_mul_ps(row_1,temp),res_2);
	temp = _MM_SWIZZLE(temp,Z,W,X,Y);
	res_1 = _mm_add_ps(_mm_mul_ps(row_2,temp),res_1);
	res_2 = _mm_sub_ps(res_2,_mm_mul_ps(row_1,temp));
	temp = _mm_mul_ps(row_0,row_2);
	temp = _MM_SWIZZLE(temp,Y,X,W,Z);
	res_1 = _mm_add_ps(_mm_mul_ps(row_3,temp),res_1);
	res_3 = _mm_sub_ps(res_3,_mm_mul_ps(row_1,temp));
	temp = _MM_SWIZZLE(temp,Z,W,X,Y);
	res_1 = _mm_sub_ps(res_1,_mm_mul_ps(row_3,temp));
	res_3 = _mm_add_ps(_mm_mul_ps(row_1,temp),res_3);
	__m128 det = _mm_mul_ps(row_0,res_0);
	det = _mm_add_ps(det,_MM_SWIZZLE(det,Y,X,W,Z));
	det = _mm_add_ss(det,_MM_SWIZZLE(det,Z,W,X,Y));
	temp = _MM_SWIZZLE(_mm_rcp_ss_nr(det),X,X,X,X);
	InvMat.row0 = _mm_mul_ps(res_0,temp);
	InvMat.row1 = _mm_mul_ps(res_1,temp);
	InvMat.row2 = _mm_mul_ps(res_2,temp);
	InvMat.row3 = _mm_mul_ps(res_3,temp);
	return InvMat;
#elif defined(V_NEON) && (defined(VPLATFORM_WINRT) || defined(VPLATFORM_IOS))
	float32x4x4_t transpose = vld4q_f32((const float32_t*)m.mat);
	float32x4_t row_0 = transpose.val[0];
	float32x4_t row_1 = vextq_f32(transpose.val[1],transpose.val[1],2);
	float32x4_t row_2 = transpose.val[2];
	float32x4_t row_3 = vextq_f32(transpose.val[3],transpose.val[3],2);
	float32x4_t temp = vmulq_f32(row_2,row_3);
	temp = vrev64q_f32(temp);
	float32x4_t res_0 = vmulq_f32(row_1,temp);
	float32x4_t res_1 = vmulq_f32(row_0,temp);
	temp = vextq_f32(temp,temp,2);
	res_0 = vnegq_f32(vmlsq_f32(res_0,row_1,temp));
	res_1 = vnegq_f32(vmlsq_f32(res_1,row_0,temp));
	res_1 = vextq_f32(res_1,res_1,2);
	temp = vmulq_f32(row_1,row_2);
	temp = vrev64q_f32(temp);
	res_0 = vmlaq_f32(res_0,row_3,temp);
	float32x4_t res_3 = vmulq_f32(row_0,temp);
	temp = vextq_f32(temp,temp,2);
	res_0 = vmlsq_f32(res_0,row_3,temp);
	res_3 = vnegq_f32(vmlsq_f32(res_3,row_0,temp));
	res_3 = vextq_f32(res_3,res_3,2);
	temp = vmulq_f32(row_3,vextq_f32(row_1,row_1,2));
	temp = vrev64q_f32(temp);
	row_2 = vextq_f32(row_2,row_2,2);
	res_0 = vmlaq_f32(res_0,row_2,temp);
	float32x4_t res_2 = vmulq_f32(row_0,temp);
	temp = vextq_f32(temp,temp,2);
	res_0 = vmlsq_f32(res_0,row_2,temp);
	res_2 = vnegq_f32(vmlsq_f32(res_2,row_0,temp));
	res_2 = vextq_f32(res_2,res_2,2);
	temp = vmulq_f32(row_0,row_1);
	temp = vrev64q_f32(temp);
	res_2 = vmlaq_f32(res_2,row_3,temp);
	res_3 = vnegq_f32(vmlsq_f32(res_3,row_2,temp));
	temp = vextq_f32(temp,temp,2);
	res_2 = vnegq_f32(vmlsq_f32(res_2,row_3,temp));
	res_3 = vmlsq_f32(res_3,row_2,temp);
	temp = vmulq_f32(row_0,row_3);
	temp = vrev64q_f32(temp);
	res_1 = vmlsq_f32(res_1,row_2,temp);
	res_2 = vmlaq_f32(res_2,row_1,temp);
	temp = vextq_f32(temp,temp,2);
	res_1 = vmlaq_f32(res_1,row_2,temp);
	res_2 = vmlsq_f32(res_2,row_1,temp);
	temp = vmulq_f32(row_0,row_2);
	temp = vrev64q_f32(temp);
	res_1 = vmlaq_f32(res_1,row_3,temp);
	res_3 = vmlsq_f32(res_3,row_1,temp);
	temp = vextq_f32(temp,temp,2);
	res_1 = vmlsq_f32(res_1,row_3,temp);
	res_3 = vmlaq_f32(res_3,row_1,temp);
	float32x4_t det = vmulq_f32(row_0,res_0);
	det = vaddq_f32(det,vextq_f32(det,det,2));
	det = vaddq_f32(det,vextq_f32(det,det,1));
	float32x2_t idet = vrcp_nr_f32(vget_low_f32(det));
	ret.row0 = vmulq_lane_f32(res_0,idet,0);
	ret.row1 = vmulq_lane_f32(res_1,idet,0);
	ret.row2 = vmulq_lane_f32(res_2,idet,0);
	ret.row3 = vmulq_lane_f32(res_3,idet,0);
#elif V_NEON
	asm volatile(
		"vld4.32  { d0, d2, d4, d6 }, [%r1]!	\n"
		"vld4.32  { d1, d3, d5, d7 }, [%r1]		\n"
		"vext.32    q1, q1, q1, #2				\n"
		"vext.32    q3, q3, q3, #2				\n"
		"vmul.f32   q8, q2, q3					\n"
		"vrev64.32  q8, q8						\n"
		"vmul.f32   q4, q1, q8					\n"
		"vmul.f32   q5, q0, q8					\n"
		"vext.32    q8, q8, q8, #2				\n"
		"vmls.f32   q4, q1, q8					\n"
		"vneg.f32   q4, q4						\n"
		"vmls.f32   q5, q0, q8					\n"
		"vneg.f32   q5, q5						\n"
		"vext.32    q5, q5, q5, #2				\n"
		"vmul.f32   q8, q1, q2					\n"
		"vrev64.32  q8, q8						\n"
		"vmla.f32   q4, q3, q8					\n"
		"vmul.f32   q7, q0, q8					\n"
		"vext.f32   q8, q8, q8, #2				\n"
		"vmls.f32   q4, q3, q8					\n"
		"vmls.f32   q7, q0, q8					\n"
		"vneg.f32   q7, q7						\n"
		"vext.32    q7, q7, q7, #2				\n"
		"vext.32    q8, q1, q1, #2				\n"
		"vmul.f32   q8, q3, q8					\n"
		"vrev64.32  q8, q8						\n"
		"vext.32    q2, q2, q2, #2				\n"
		"vmla.f32   q4, q2, q8					\n"
		"vmul.f32   q6, q0, q8					\n"
		"vext.f32   q8, q8, q8, #2				\n"
		"vmls.f32   q4, q2, q8					\n"
		"vmls.f32   q6, q0, q8					\n"
		"vneg.f32   q6, q6						\n"
		"vext.32    q6, q6, q6, #2				\n"
		"vmul.f32   q8, q0, q1					\n"
		"vrev64.32  q8, q8						\n"
		"vmla.f32   q6, q3, q8					\n"
		"vmls.f32   q7, q2, q8					\n"
		"vneg.f32   q7, q7						\n"
		"vext.f32   q8, q8, q8, #2				\n"
		"vmls.f32   q6, q3, q8					\n"
		"vneg.f32   q6, q6						\n"
		"vmls.f32   q7, q2, q8					\n"
		"vmul.f32   q8, q0, q3					\n"
		"vrev64.32  q8, q8						\n"
		"vmls.f32   q5, q2, q8					\n"
		"vmla.f32   q6, q1, q8					\n"
		"vext.f32   q8, q8, q8, #2				\n"
		"vmla.f32   q5, q2, q8					\n"
		"vmls.f32   q6, q1, q8					\n"
		"vmul.f32   q8, q0, q2					\n"
		"vrev64.32  q8, q8						\n"
		"vmla.f32   q5, q3, q8					\n"
		"vmls.f32   q7, q1, q8					\n"
		"vext.f32   q8, q8, q8, #2				\n"
		"vmls.f32   q5, q3, q8					\n"
		"vmla.f32   q7, q1, q8					\n"
		"vmul.f32   q0, q0, q4					\n"
		"vext.32    q1, q0, q0, #2				\n"
		"vadd.f32   q0, q0, q1					\n"
		"vext.32    q1, q0, q0, #1				\n"
		"vadd.f32   q0, q0, q1					\n"
		"vrecpe.f32 q1, q0						\n"
		"vrecps.f32 q2, q1, q0					\n"
		"vmul.f32   q1, q1, q2					\n"
		"vrecps.f32 q2, q1, q0					\n"
		"vmul.f32   q8, q1, q2					\n"
		"vmul.f32   q0, q4, q8					\n"
		"vmul.f32   q1, q5, q8					\n"
		"vmul.f32   q2, q6, q8					\n"
		"vmul.f32   q3, q7, q8					\n"
		"vstm      %r0, { q0 - q3 }				\n"
		: : "r"(ret.mat), "r"(m.mat)
		: "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8"
		);
#else
	// 事实上,我们绝大多数的逆操作都能成功. 
	// 注意到我们GetDeterm() 操作中计算了一部分余子式,如果逆操作成功,那么GetDeterm就浪费了.
	// NOTE: 下面的实现是以矩阵可逆为前提的, 对不可逆矩阵将返回单位矩阵.
	VMatrix4X4 InvMat;
	float tmp[12];
	tmp[0] = m33 * m44; tmp[1] = m43 * m34; tmp[2] = m23 * m44; tmp[3] = m24 * m43;
	tmp[4] = m23 * m34; tmp[5] = m33 * m24; tmp[6] = m13 * m44; tmp[7] = m43 * m14;
	tmp[8] = m13 * m34; tmp[9] = m33 * m14; tmp[10]= m13 * m24; tmp[11]= m23 * m14;	// # 12*

	// 计算余子式 (倒置伴随矩阵)
	InvMat.m11 = m22*(tmp[0] - tmp[1]) + m32*(tmp[3] - tmp[2]) + m42*(tmp[4] - tmp[5]);  // 3* 3- 2+
	InvMat.m12 = m12*(tmp[1] - tmp[0]) + m32*(tmp[6] - tmp[7]) + m42*(tmp[9] - tmp[8]);
	InvMat.m13 = m12*(tmp[2] - tmp[3]) + m22*(tmp[7] - tmp[6]) + m42*(tmp[10] - tmp[11]);
	InvMat.m14 = m12*(tmp[5] - tmp[4]) + m22*(tmp[8] - tmp[9]) + m32*(tmp[11] - tmp[10]);
	InvMat.m21 = m21*(tmp[1] - tmp[0]) + m31*(tmp[2] - tmp[3]) + m41*(tmp[5] - tmp[4]);
	InvMat.m22 = m11*(tmp[0] - tmp[1]) + m31*(tmp[7] - tmp[6]) + m41*(tmp[8] - tmp[9]);
	InvMat.m23 = m11*(tmp[3] - tmp[2]) + m21*(tmp[6] - tmp[7]) + m41*(tmp[11] - tmp[10]);
	InvMat.m24 = m11*(tmp[4] - tmp[5]) + m21*(tmp[9] - tmp[8]) + m31*(tmp[10] - tmp[11]);
	//36* 24- 16+
	tmp[0] = m31 * m42; tmp[1] = m41 * m32; tmp[2] = m21 * m42; tmp[3] = m41 * m22;
	tmp[4] = m21 * m32; tmp[5] = m31 * m22; tmp[6] = m11 * m42; tmp[7] = m41 * m12;
	tmp[8] = m11 * m32; tmp[9] = m31 * m12; tmp[10]= m11 * m22; tmp[11]= m21 * m12;

	InvMat.m31 = m24*(tmp[0] - tmp[1]) + m34*(tmp[3] - tmp[2]) + m44*(tmp[4] - tmp[5]);
	InvMat.m32 = m14*(tmp[1] - tmp[0]) + m34*(tmp[6] - tmp[7]) + m44*(tmp[9] - tmp[8]);
	InvMat.m33 = m14*(tmp[2] - tmp[3]) + m23*(tmp[7] - tmp[6]) + m44*(tmp[10] - tmp[11]);
	InvMat.m34 = m14*(tmp[5] - tmp[4]) + m24*(tmp[8] - tmp[9]) + m34*(tmp[11] - tmp[10]);
	InvMat.m41 = m23*(tmp[1] - tmp[0]) + m33*(tmp[2] - tmp[3]) + m43*(tmp[5] - tmp[4]);
	InvMat.m42 = m33*(tmp[7] - tmp[6]) + m13*(tmp[0] - tmp[1]) + m43*(tmp[8] - tmp[9]);
	InvMat.m43 = m23*(tmp[6] - tmp[7]) + m13*(tmp[3] - tmp[2]) + m43*(tmp[11] - tmp[10]);
	InvMat.m44 = m13*(tmp[4] - tmp[5]) + m33*(tmp[10] - tmp[11]) + m23*(tmp[9] - tmp[8]);
	// 72* 48- 32+
	float fDet=(m11*InvMat.m11 + m21*InvMat.m12 + m31*InvMat.m13 + m41*InvMat.m14);		// 4* 3+
	if (VAbs(fDet) < 0.000001f)
	{
		InvMat.SetIdentity();
		return InvMat;
	}

	fDet = 1.0f/fDet;
	InvMat.m11*= fDet;InvMat.m12*= fDet;InvMat.m13*= fDet;InvMat.m14*= fDet;
	InvMat.m21*= fDet;InvMat.m22*= fDet;InvMat.m23*= fDet;InvMat.m24*= fDet;
	InvMat.m31*= fDet;InvMat.m32*= fDet;InvMat.m33*= fDet;InvMat.m34*= fDet;
	InvMat.m41*= fDet;InvMat.m42*= fDet;InvMat.m43*= fDet;InvMat.m44*= fDet;//16*
	return InvMat;
#endif
}

inline void VMatrix4X4::SetTranslate(float tx,float ty, float tz)
{
	m11 = 1.0f;		m12 = 0.0f;		m13 = 0.0f;		m14 = 0.f;
	m21 = 0.0f;		m22 = 1.0f;		m23 = 0.0f;		m24 = 0.f;
	m31 = 0.0f;		m32 = 0.0f;		m33 = 1.0f;		m34 = 0.0f;
	m41 = tx;		m42 = ty;		m43 = tz;		m44 = 1.0f;
}

inline void VMatrix4X4::SetScale(float x, float y, float z)
{
	m11 = x;		m12 = 0.0f;		m13 = 0.0f;		m14 = 0.f;
	m21 = 0.0f;		m22 = y;		m23 = 0.0f;		m24 = 0.f;
	m31 = 0.0f;		m32 = 0.0f;		m33 = z;		m34 = 0.0f;
	m41 = 0.0f;		m42 = 0.0f;		m43 = 0.0f;		m44 = 1.0f;
}

inline void VMatrix4X4::SetRotateFromAxisAngle(const VVector3& _Axis, float Angle)
{
	float s,c;
	VSinCos(Angle,s,c);
	float ic = 1.0f - c;
	VVector3 Axis(_Axis);
	Axis.Normalize();
	float xy = Axis.x * Axis.y;
	float yz = Axis.y * Axis.z;
	float zx = Axis.z * Axis.x;
	float xs = Axis.x * s;
	float ys = Axis.y * s;
	float zs = Axis.z * s;
	m11 = c + ic * Axis.x * Axis.x;		m12 = ic * xy + zs;					m13 = ic * zx - ys;
	m21 = ic * xy - zs;					m22 = c + ic * Axis.y * Axis.y;		m23 = ic * yz + xs;
	m31 = ic * zx + ys;					m32 = ic * yz - xs;					m33 = c + ic * Axis.z * Axis.z;
	m14 = m24 = m34 = m41 = m42 = m43 = 0.0f;	m44 = 1.0f;
}

inline bool VMatrix4X4::SetRotateFromTwoVectors(const VVector3& _From, const VVector3& _To)
{
	// 内部展开AxisAngle -> Quat -> Matrix 的过程, 减少计算量.
	// GPG1 2.10
	VVector3 From(_From), To(_To);
	From.Normalize();
	To.Normalize();
	VVector3 Axis = From^To;	// cross
	if (Axis.LengthSqr() < VEPS)
	{
		SetIdentity();
		return false;
	}
	float Cos = From|To;
	float Sin22 = ( 1.0f - Cos );
	float xy = Axis.x * Axis.y;
	float xz = Axis.x * Axis.z;
	float yz = Axis.y * Axis.z;
	
	m11 = Sin22 * Axis.x * Axis.x + Cos;		m12 = Sin22 * xy + Axis.z;				m13 = Sin22 * xz - Axis.y;
	m21 = Sin22 * xy - Axis.z;					m22 = Sin22 *  Axis.y * Axis.y + Cos;	m23 = Sin22 * yz + Axis.x;
	m31 = Sin22 * xz + Axis.y;					m32 = Sin22 * yz - Axis.x;				m33 = Sin22 * Axis.z * Axis.z + Cos;
	m14 = m24 = m34 = m41 = m42 = m43 = 0.0f;	m44 = 1.0f;
	return true;
}

inline void VMatrix4X4::SetRotateFromQuat(const VQuat& Quat)
{
	Quat.ToRotateMatrix(*this);
}

inline void VMatrix4X4::SetRotateX(float Angle)
{
	m11 = 1.0f; m12 = 0.0f;  m13 = 0.0f;	m14 = 0.f;
	m21 = 0.0f;								m24 = 0.f;
	m31 = 0.0f;								m34 = 0.0f;
	m41 = 0.0f; m42 = 0.0f;  m43 = 0.0f;	m44 = 1.0f;
	float c,s;
	VSinCos(Angle,s,c);
	m22 = m33 = c;
	m23 = s;
	m32 = -s;
}
inline void VMatrix4X4::SetRotateY(float Angle)
{
	m12 = 0.0f;					m14 = 0.f;
	m21 = 0.0f; m22 = 1.0f;  m23 = 0.0f;  m24 = 0.f;
	m32 = 0.0f;					m34 = 0.0f;
	m41 = 0.0f; m42 = 0.0f;  m43 = 0.0f;  m44 = 1.0f;
	float c,s;
	VSinCos(Angle,s,c);
	m11 = m33 = c;
	m13 = -s;
	m31 = s;
}
inline void VMatrix4X4::SetRotateZ(float Angle)
{
	m13 = 0.0f;  m14 = 0.f;
	m23 = 0.0f;  m24 = 0.f;
	m31 = 0.0f; m32 = 0.0f;  m33 = 1.0f;  m34 = 0.0f;
	m41 = 0.0f; m42 = 0.0f;  m43 = 0.0f;  m44 = 1.0f;
	float c,s;
	VSinCos(Angle,s,c);
	m11 = m22 = c;
	m12 = s;
	m21 = -s;
}

inline void VMatrix4X4::RemoveScale()
{
	// 将3个基底矢量归一化.
	float SqrLen;
	for(int i=0; i<3; i++)
	{
		SqrLen = (m[i][0] * m[i][0]) + (m[i][1] * m[i][1]) + (m[i][2] * m[i][2]);
		if(SqrLen > VEPS)
		{
			SqrLen = 1.f/VSqrt(SqrLen);
			m[i][0] *= SqrLen; 
			m[i][1] *= SqrLen; 
			m[i][2] *= SqrLen; 
		}
	}
}

inline VVector3 VMatrix4X4::GetScale() const
{
	VVector3 Scale(1.0f);

	const float LenX = (m[0][0] * m[0][0]) + (m[0][1] * m[0][1]) + (m[0][2] * m[0][2]);
	const float LenY = (m[1][0] * m[1][0]) + (m[1][1] * m[1][1]) + (m[1][2] * m[1][2]);
	const float LenZ = (m[2][0] * m[2][0]) + (m[2][1] * m[2][1]) + (m[2][2] * m[2][2]);

	if( LenX > VEPS )
	{
		Scale.x = VSqrt(LenX);
	}


	if( LenY > VEPS )
	{
		Scale.y = VSqrt(LenY);
	}


	if( LenZ > VEPS )
	{
		Scale.z = VSqrt(LenZ);
	}

	return Scale;
}


inline void VMatrix4X4::TransformVec3(VVector3& Out, const VVector3& V) const
{
	// TODO SSE opt

	float x,y,z;	// 避免out与v 相同
	x = V.x * m11 + V.y * m21 + V.z * m31 + m41;
	y = V.x * m12 + V.y * m22 + V.z * m32 + m42;
	z = V.x * m13 + V.y * m23 + V.z * m33 + m43;
	Out.x = x; Out.y= y; Out.z = z;
}

inline void VMatrix4X4::TransformVec4(VVector4& Out, const VVector4& V) const
{
#if V_SSE
	__m128 temp =		   _mm_mul_ps(_MM_SWIZZLE(V.vec,X,X,X,X), row0);			// x * [m11 m12 m13 m14]'
	temp =		_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(V.vec,Y,Y,Y,Y), row1), temp);		// y * [m21 m22 m23 m24]'
	temp =		_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(V.vec,Z,Z,Z,Z), row2), temp);		// z * [m31 m32 m33 m34]'
	Out.vec =	_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(V.vec,W,W,W,W), row3), temp);		// w * [m41 m42 m43 m44]'
#elif V_NEON 
	float32x2_t t0 = vget_low_f32(V.vec);
	float32x2_t t1 = vget_high_f32(V.vec);
	V.vec = vmulq_lane_f32(row0, t0, 0);
	V.vec = vmlaq_lane_f32(temp, row1, t0, 1 );
	V.vec = vmlaq_lane_f32(temp, row2, t1, 0 );
	V.vec = vmlaq_lane_f32(temp, row3, t1, 1 );
#else
	float x,y,z,w;
	x = V.x * m11 + V.y * m21 + V.z * m31 + V.w * m41;
	y = V.x * m12 + V.y * m22 + V.z * m32 + V.w * m42;
	z = V.x * m13 + V.y * m23 + V.z * m33 + V.w * m43;
	w = V.x * m14 + V.y * m24 + V.z * m34 + V.w * m44; 
	Out.x = x; Out.y= y; Out.z = z; Out.w = w;
#endif 
}


inline void VMatrix4X4::InvTransformVec3NoScale(VVector3& Out, const VVector3& V) const
{
	// 当需要将一个点V变换到当前空间M的时候,总是 V' = V * Inv(M)
	// 但是计算Inv(M)将是一个非常繁重的运算, 但当M 仅为Rotate, Translate组合而成的正交矩阵时候(没有Scale, 包括uniform scale,un-uniform scale).有个特性可以利用.
	//  Inv(R) = Transpose(R)  Inv(T) = -T (R 为rotation matrix , T: translation matrix)
	//  | R 0 |   |R 0|  |0 0|
	//  |     | = |   | *|   |   -> M = R * T
	//  | T 1 |   |0 1|  |T 1|
	// Inv(M) = Inv(R * T) = Inv(T) * Inv(R) = (-T) * (Transpose(R))
	// V' = V * Inv(M) = V *(-T)*(Transpose(R)) 
	// 这时变换将会非常简单.
	VVector3 t;
	// t = V * (-T) 
	t.x = V.x - m41; t.y = V.y - m42; t.z = V.z - m43;
	// t * Transpose(R)
	Out.x = t.x * m11 + t.y * m12 + t.z * m13;
	Out.y = t.x * m21 + t.y * m22 + t.z * m23;
	Out.z = t.x * m31 + t.y * m32 + t.z * m33;
}

// 变换方向矢量
inline void VMatrix4X4::TransformNormal(VVector3& Out, const VVector3& V) const
{
	// N' = N * Inv(Transpose(M))
	// 但是对于没有non-uniform scale 的矩阵. Transpose(M) = Inv(M) -> N' = N * Inv(Inv(M)) = N * M 
	float x,y,z;	// 避免out与v 相同
	x = V.x * m11 + V.y * m21 + V.z * m31;
	y = V.x * m12 + V.y * m22 + V.z * m32;
	z = V.x * m13 + V.y * m23 + V.z * m33;
	Out.x = x; Out.y= y; Out.z = z;
}

inline void VMatrix4X4::TransformNormal(VByteVector& Out, const VByteVector& V) const
{
	VVector3 Normal((VVector3)V);
	TransformNormal(Normal, Normal);
	Out = Normal;
}

inline void VMatrix4X4::InvTransformNormalNoScale(VVector3& Out, const VVector3& V) const
{
	VVector3 t(Out);
	Out.x = t.x * m11 + t.y * m12 + t.z * m13;
	Out.y = t.x * m21 + t.y * m22 + t.z * m23;
	Out.z = t.x * m31 + t.y * m32 + t.z * m33;
}

inline void VMatrix4X4::LookAt(const VVector3& Eye, const VVector3& At, const VVector3& Up)
{
	VVector3 Z = Eye - At;	Z.Normalize();
	VVector3 X = Up.Cross(Z); X.Normalize();
	VVector3 Y = Z.Cross(X); Y.Normalize();
	m11 = X.x;	m12 = Y.x;	m13 = Z.x;	m14 = 0.0f; 
	m21 = X.y;	m22 = Y.y;	m23 = Z.y;	m24 = 0.0f;
	m31 = X.z;	m32 = Y.z;	m33 = Z.z;	m34 = 0.0f;
	m41 = -X.Dot(Eye);
	m42 = -Y.Dot(Eye);	
	m43 = -Z.Dot(Eye);	
	m44 = 1.0f;
}

inline void VMatrix4X4::Perspective(float Fov, float Aspect, float Near, float Far)
{
	float Cot = 1.0f / VTan(VDEG_RAD(Fov * 0.5f));
	float DeltaZ = 1.0f / (Near - Far);
	m11 = Cot / Aspect; m12 = 0.0f;		m13 = 0.0f;					m14 = 0.f;
	m21 = 0.0f;			m22 = Cot;		m23 = 0.0f;					m24 = 0.f;
	m31 = 0.0f;			m32 = 0.0f;		m33 = Far * DeltaZ;			m34 = -1.0f;
	m41 = 0.0f;			m42 = 0.0f;		m43 = Far * Near * DeltaZ;	m44 = 0.0f;
}

inline void VMatrix4X4::OrthoOffCenter(float Left, float Right, float Bottom, float Top, float Near, float Far)
{
	float R_L = 1.0f/(Right - Left);
	float T_B = 1.0f/(Top - Bottom);
	float N_F = 1.0f/(Near - Far);
	m11 = 2.0f * R_L;			m12 = 0.0f;					m13 = 0.0f;					m14 = 0.f;
	m21 = 0.0f;					m22 = 2.0f*T_B;				m23 = 0.0f;					m24 = 0.f;
	m31 = 0.0f;					m32 = 0.0f;					m33 = N_F;					m34 = 0.0f;
	m41 = -(Left+Right)*R_L;	m42 = -(Bottom + Top)*T_B;	m43 = Near * N_F;			m44 = 1.0f;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//#include <d3d9.h>
//#include <d3dx9.h>
//
//inline VVector3& VVec3Transform(VVector3& Out, VVector3& V, VMatrix4X4& Mat)
//{
//	Mat.TransformVec3(Out, V);
//	return Out;
//}

#pragma warning(pop)
#endif 

