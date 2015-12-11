#ifndef __VBOUNDS_H__
#define __VBOUNDS_H__
#pragma once
#include "VMath.h"

enum EVOverlapResult
{
	VOR_OUTSIDE = 0,		// full outside
	VOR_OVERLAP,			// partial inside
	VOR_INSIDE,				// full inside
};

class VBoundSphere;
class VBound;
class VBoundFrustum;

class VCoreApi VBoundBox
{
public:
	VVector3 Min,Max;
	inline VBoundBox() {}
	inline VBoundBox(const VVector3& _Min, const VVector3& _Max):Min(_Min),Max(_Max) {}
	inline VBoundBox(const VVector3* Points, INT NumPoints)
	{
	}
	inline VBoundBox(const VBoundBox& Box, const VMatrix4X4& Mat):Min(Box.Min),Max(Box.Max)
	{
		Transform(Mat);
	}

	inline bool IsValid() const { return Min.x < Max.x && Min.y < Max.y && Min.z < Max.z; }
	inline bool IsValidFast() const { return Min.x < Max.x; }
	//! 设置为空
	inline void SetEmpty()	{	Min = VVec3_INF; Max = -VVec3_INF;	}
	//! 设置为无穷大
	inline void SetInfinite()	{	Min = -VVec3_INF; Max = VVec3_INF;	}
	inline void Clear() { Min.x = VINFINITY; Max.x = -VINFINITY;}


	// 扩展包围盒以包含指定顶点.
	inline void SetInclude(const VVector3& Vertex)
	{
		if (IsValidFast())
		{
			VASSERT(IsValid());
#if 0
			// a bit slow .... 
			__m128 min = _mm_loadu_ps((float*)&Min);
			__m128 max = _mm_loadu_ps((float*)&Max);
			__m128 vertex =_mm_loadu_ps((float*)&Vertex);
			min = _mm_min_ps(min,vertex);
			max = _mm_max_ps(max,vertex);
			_mm_storeu_ps(Min, min);
			_mm_storeu_ps(Max, max);
#else
			Min.x = VMin(Min.x, Vertex.x);
			Min.y = VMin(Min.y, Vertex.y);
			Min.z = VMin(Min.z, Vertex.z);
			Max.x = VMin(Max.x, Vertex.x);
			Max.y = VMin(Max.y, Vertex.y);
			Max.z = VMin(Max.z, Vertex.z);
#endif
		}else
		{
			Min = Vertex - VVec3_EPS; Max = Vertex + VVec3_EPS;
		}
	}

	inline void SetInclude(const VVector3* Points, UINT NumPoints)
	{
		for (UINT Index = 0; Index < NumPoints; ++Index)
		{
			SetInclude(Points[Index]);
		}
	}

	// 扩展包围盒以包含指定包围盒.
	inline void SetInclude(const VBoundBox& Box)
	{
		bool BoxValid = Box.IsValidFast();
		if (BoxValid && IsValidFast())
		{
			VASSERT(Box.IsValid() && IsValid());		// check for safe.
			Min.x = VMin(Min.x, Box.Min.x);
			Min.y = VMin(Min.y, Box.Min.y);
			Min.z = VMin(Min.z, Box.Min.z);
			Max.x = VMin(Max.x, Box.Max.x);
			Max.y = VMin(Max.y, Box.Max.y);
			Max.z = VMin(Max.z, Box.Max.z);
		}else if (BoxValid)
		{
			Min = Box.Min; Max = Box.Max;
		}
	}

	inline void SetInclude(const VBoundSphere& Sphere);
	inline void SetInclude(const VBound& Bound);

	inline VVector3 GetCenter() const { return (Min + Max)*0.5f; }

	// 判断点是否在包围盒里面?
	inline bool IsInside(const VVector3& Point) const 
	{
		return Point.x > Min.x && Point.x < Max.x && Point.y > Min.y && Point.y < Max.y && Point.z > Min.z && Point.z < Max.z;
	}

	// 判断Box 是否与包围盒相交
	inline bool Intersect(const VBoundBox& Box) const 
	{
		if(Min.x > Box.Max.x || Max.x < Box.Min.x) return false;
		if(Min.y > Box.Max.y || Max.y < Box.Min.y) return false;
		if(Min.z > Box.Max.z || Max.z < Box.Min.z) return false;
		return true;
	}
	// 判断Sphere是否与包围盒相交(Fast, but not )
	inline bool Intersect(const VBoundSphere& Sphere) const;

	// 
	inline bool Intersect(const VBound& Bound) const;


	inline EVOverlapResult OverlapCheck(const VBoundBox& Box) const;

	inline void Transform(const VMatrix4X4& Mat) 
	{

#if V_SSE
		__m128 min = _mm_loadu_ps((float*)&Min);
		__m128 max = _mm_loadu_ps((float*)&Max);
		__m128 zero = _mm_setzero_ps();
		__m128 Center = _mm_mul_ps(_mm_add_ps(min,max),_mm_set1_ps(0.5f));
		__m128 Size = _mm_sub_ps(max,Center);
		__m128 temp,tempr;
		//Abs transform Size
		tempr = _mm_max_ps(_mm_sub_ps(zero, Mat.row0), Mat.row0);
		temp = _mm_mul_ps(_MM_SWIZZLE(Size,X,X,X,W), tempr);							// x * Abs([m11 m12 m13 m14])'
		tempr = _mm_max_ps(_mm_sub_ps(zero, Mat.row1), Mat.row1);
		temp =	_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(Size,Y,Y,Y,W), tempr), temp);		// y * Abs([m21 m22 m23 m24])'
		tempr = _mm_max_ps(_mm_sub_ps(zero, Mat.row2), Mat.row2);
		Size =	_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(Size,Z,Z,Z,W), tempr), temp);		// z * Abs([m31 m32 m33 m34])'

		// transform Center
		temp = _mm_mul_ps(_MM_SWIZZLE(Center,X,X,X,W), Mat.row0);				// x * [m11 m12 m13 m14]'
		temp =	_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(Center,Y,Y,Y,W), Mat.row1), temp);		// y * [m21 m22 m23 m24]'
		temp =	_mm_add_ps(_mm_mul_ps(_MM_SWIZZLE(Center,Z,Z,Z,W), Mat.row2), temp);		// z * [m31 m32 m33 m34]'
		Center = _mm_add_ps(Mat.row3, temp);		// + [m41 m42 m43 m44]'

		min = _mm_sub_ps(Center, Size);
		max = _mm_add_ps(Center, Size);
		VVMStoreFloat3((float*)&Min, min);
		VVMStoreFloat3((float*)&Max, max);
#elif V_NEON
		TODO
#else 
		// 确保没有透视变换.
		VASSERT(Mat.m14 == 0 && Mat.m24 == 0 && Mat.m34 == 0 && Mat.m44 == 1);
		VVector3 Center = (Min + Max) * 0.5f;
		VVector3 Size = Max - Center;		// HalfSize
		VVector3 NewSize;
		NewSize.x = Size.x * VAbs(Mat.m11) + Size.y * VAbs(Mat.m21) + Size.z* VAbs(Mat.m31);
		NewSize.y = Size.x * VAbs(Mat.m12) + Size.y * VAbs(Mat.m22) + Size.z* VAbs(Mat.m32);
		NewSize.z = Size.x * VAbs(Mat.m13) + Size.y * VAbs(Mat.m23) + Size.z* VAbs(Mat.m33);
		Mat.TransformVec3(Center, Center);
		Min = Center-NewSize;
		Max = Center+NewSize;
#endif 
	}

	inline bool RayIntersect(const VVector3& Point, const VVector3& Direction) const;

	/// 计算与点的距离
	inline float DistanceSqr(const VVector3& Point) const;
	inline float Distance(const VVector3& Point) const { return VSqrt(DistanceSqr(Point));}
};

class VCoreApi VBoundSphere
{
public:
	VVector3	Center;
	float		Radius;
	inline VBoundSphere():Radius(-1.0f){}
	inline VBoundSphere(const VVector3& _Center, float _Radius):Center(_Center),Radius(_Radius)
	{
	}
	inline VBoundSphere(const VBoundSphere& Sphere):Center(Sphere.Center),Radius(Sphere.Radius){}

	inline VBoundSphere(const VVector3* Points, int NumPoints)
	{
		Center = Points[0];
		for (int p =1; p < NumPoints; ++p)
		{
			Center += Points[p];
		}
		Center /= (float)NumPoints;
		
		Radius = 0.0f;
		float DistanceSqr;
		for (int p =0; p < NumPoints; ++p)
		{
			DistanceSqr = (Points[p] - Center).LengthSqr();
			if (DistanceSqr > Radius)
			{
				Radius = DistanceSqr;
			}
		}
		Radius = VSqrt(DistanceSqr) + 0.001f;
	}
//	VBoundSphere(const VBoundSphere& bs);
	VBoundSphere(const VBoundSphere& bs,const VMatrix4X4& Transform);
	explicit VBoundSphere(const VBoundBox& bb);
	inline ~VBoundSphere(){}


	inline bool IsValid() const { return Radius >= VEPS; }
	inline void Set(const VVector3& _Center, float _Radius) 
	{
		Center = _Center; Radius = _Radius;
	}

	/// 从点集合构建一个紧凑包围球(包围半径最小), 但是会消耗更多性能. 
	inline void SetTight(const VVector3* Points, int NumPoints);

	void Set(const VBoundSphere& VBoundSphere);
	void Set(const VBoundSphere& VBoundSphere,const VMatrix4X4& Transform);
	void Set(const VBoundBox& VBoundBox);

	// transformation
	inline void SetTransform(const VMatrix& Transform)
	{
		// TODO SSE 
		Transform.TransformVec3(Center, Center);
		// scale radius. find the max scale.
		VVector3 Scale = Transform.GetScale();
		float MaxScale = VMax(Scale.x, VMax(Scale.y, Scale.z));
		Radius = Radius * MaxScale;
	}
//	void setTransform(const dmat4 &transform);

	// compare
	//int compare(const VBoundSphere &bs) const;
	//inline int operator==(const VBoundSphere &bs) const { return compare(bs); }
	//inline int operator!=(const VBoundSphere &bs) const { return !compare(bs); }

	// expand
	void expand(const VVector3& point);
	void expand(const VVector3 *Points,int num_points);
	void expand(const VBoundSphere &bs);
	void expand(const VBoundBox &bb);

	// radius expand
	void expandRadius(const VVector3& point);
	void expandRadius(const VVector3 *Points,int num_points);
	void expandRadius(const VBoundSphere &bs);
	void expandRadius(const VBoundBox &bb);

	// is Point inside this
	inline bool Inside(const VVector3& point) const
	{
		return (point - Center).LengthSqr() < (Radius * Radius);
	}
	
	/// Sphere with point and radius inside this ?
	inline bool Inside(const VVector3& point,float radius) const
	{
		radius += Radius;
		return (Center - point).LengthSqr() < (radius * radius);
	}

	/// Box with min max inside this ?
	inline bool Inside(const VVector3& min,const VVector3& max) const
	{
		// SSE ??
		if(Center.x + Radius < min.x || Center.x - Radius > max.x) return false;
		if(Center.y + Radius < min.y || Center.y - Radius > max.y) return false;
		if(Center.z + Radius < min.z || Center.z - Radius > max.z) return false;
		return true;
	}

	/// Sphere Inside this ??? 
	inline bool Inside(const VBoundSphere &bs) const
	{
		return Inside(bs.Center, bs.Radius);
	}
	/// BoundBox inside this?
	inline bool Inside(const VBoundBox &bb) const
	{
		return Inside(bb.Min, bb.Max);
	}

	inline bool InsideAll(const VVector3& Point, float radius) const 
	{
		radius = Radius - radius;
		if (radius > 0.0f)
		{
			return (Center - Point).LengthSqr() < (radius * radius);
		}
		return false;
	}
	// bs full inside this?
	inline bool InsideAll(const VBoundSphere &bs) const
	{
		return InsideAll(bs.Center, bs.Radius);
	}

	inline bool InsideAll(const VVector3& min, const VVector3& max) const
	{
		// check the 8 points inside sphere.
		if(!Inside(VVector3(min.x,min.y,min.z))) return false;
		if(!Inside(VVector3(max.x,min.y,min.z))) return false;
		if(!Inside(VVector3(min.x,max.y,min.z))) return false;
		if(!Inside(VVector3(max.x,max.y,min.z))) return false;
		if(!Inside(VVector3(min.x,min.y,max.z))) return false;
		if(!Inside(VVector3(max.x,min.y,max.z))) return false;
		if(!Inside(VVector3(min.x,max.y,max.z))) return false;
		if(!Inside(VVector3(max.x,max.y,max.z))) return false;
		return true;
	}

	// bb full inside this?
	inline bool InsideAll(const VBoundBox &bb) const
	{	
		return InsideAll(bb.Min, bb.Max);
	}

	int insideAllValid(const VBoundSphere &bs) const;
	int insideAllValid(const VBoundBox &bb) const;

	// intersections
	int rayIntersection(const VVector3& point,const VVector3& direction) const;
	int getIntersection(const VVector3& p0,const VVector3& p1) const;

	int rayIntersectionValid(const VVector3& point,const VVector3& direction) const;
	int getIntersectionValid(const VVector3& p0,const VVector3& p1) const;

	// distance
	float distance() const;
	float distance(const VVector3& point) const;

	float distanceValid() const;
	float distanceValid(const VVector3& point) const;

	//inline EVOverlapResult OverlapCheck(const VBoundBox& Box) const;
};

class VCoreApi VBound
{
public:
	VVector3	Center;
	VVector3	HalfSize;
	float		Radius;
	inline VBound(){}
	inline VBound(const VVector3& _Center, float _Radius)
		:Center(_Center),HalfSize(_Radius*0.5f),Radius(_Radius)
	{
	}

	inline VBound(const VBoundSphere& BoundSphere)
		:Center(BoundSphere.Center),HalfSize(BoundSphere.Radius * 0.5f),Radius(BoundSphere.Radius)
	{

	}

	inline VBound(const VBoundBox& BoundBox)
	{
		Center = (BoundBox.Min + BoundBox.Max)*0.5f;
		HalfSize = (BoundBox.Max - BoundBox.Min) * 0.5f;
		Radius = VMax(HalfSize.x, VMax(HalfSize.y, HalfSize.z));
	}


	inline VBoundBox GetBoundBox() const 
	{
		return VBoundBox(Center - HalfSize, Center + HalfSize);
	}

	inline VBoundSphere GetBoundSphere() const 
	{
		return VBoundSphere(Center, Radius);
	}

	inline float DistanceSqr(const VVector3& Point) const 
	{
		return (Point - Center).LengthSqr();
	}
};

#pragma warning(push)
#pragma warning(disable:4800)
//
//
// TODO 移动到Engine 中.
class VCoreApi VBoundFrustum 
{		
public:
	VBoundFrustum();
	VBoundFrustum(const VMatrix& projection,const VMatrix& modelview);
	VBoundFrustum(const VBoundFrustum& bf);
	VBoundFrustum(const VBoundFrustum& bf,const VMatrix& itransform);
	~VBoundFrustum();

	inline void Clear() { Portals.Clear(); }

	// from proj and view matrix.
	void Set(const VMatrix& View, const VMatrix& Projection);


	// Overlap Check with frustum plane
	EVOverlapResult OverlapCheckPlane(const VBoundSphere& Sphere) const;

	// Overlap check with portal clip planes. 
	// if there is no portal, return OUTSIDE. 
	EVOverlapResult OverlapCheckPortal(const VBoundSphere& Sphere) const;

	// Overlap Check with frustum plane
	EVOverlapResult OverlapCheckPlane(const VBoundBox& Box) const;

	// Overlap check with portal clip planes. 
	// if there is no portal, return OUTSIDE. 
	EVOverlapResult OverlapCheckPortal(const VBoundBox& Box) const;


	inline bool Inside(const VBoundSphere &bs) const
	{
		return InsidePlanes(bs.Center, bs.Radius);
	}
	inline bool Inside(const VBoundBox &bb) const
	{
		return InsidePlanes(bb.Min, bb.Max);
	}
	bool Inside(const VBoundFrustum& bf) const;

	inline bool InsideFast(const VBoundSphere &bs) const
	{
		return InsidePlanesFast(bs.Center, bs.Radius);
	}
	inline bool InsideFast(const VBoundBox& bb, int NumPlanes = 4) const
	{
		return InsidePlanesFast(bb.Min, bb.Max, NumPlanes);
	}

	// transformation
	void setITransform(const VMatrix& itransform);

	// 添加portal. 如果portal 不在包围体内, 将不会添加
	void AddPortal(const VVector3 *Points,int num_points,const VMatrix& transform);
	inline void RemovePortals(int Num)
	{
		//VASSERT();
		//Portals.Remove();
	}

	// expand
	void expand(float radius);

	// Inside Points
	inline bool Inside(const VVector3& point) const
	{
		if(!InsidePortals(point)) 
			return false;
		return InsidePlanes(point);
	}
	inline bool Inside(const VVector3& point,float radius) const
	{
		if(!InsidePortals(point,radius)) return false;
		return InsidePlanes(point,radius);
	}
	bool Inside(const VVector3& min,const VVector3& max) const
	{
		if(!InsidePortals(min,max)) return false;
		return InsidePlanes(min,max);
	}

	// 
	bool Inside(const VVector3* Points,int num) const
	{
		if(!InsidePortals(Points,num)) return false;
		return InsidePlanes(Points,num);
	}

	// Fast Version of Inside Check.
	// Ignore near/far plane check. 
	int insideFast(const VVector3& point) const;
	int insideFast(const VVector3& point,float radius) const;
	int insideFast(const VVector3& min,const VVector3& max) const;
	int insideFast(const VVector3 *Points,int num) const;

	// Inside bounds
	

	int insideValid(const VBoundSphere &bs) const;
	int insideValid(const VBoundBox &bb) const;
	int insideValid(const VBoundFrustum& bf) const;

	int insideValidFast(const VBoundSphere &bs) const;
	int insideValidFast(const VBoundBox &bb) const;
	int insideValidFast(const VBoundFrustum& bf) const;

	// Inside all bounds
	int insideAll(const VBoundSphere &bs) const;
	int insideAll(const VBoundBox &bb) const;
	int insideAll(const VBoundFrustum& bf) const;

	int insideAllValid(const VBoundSphere &bs) const;
	int insideAllValid(const VBoundBox &bb) const;
	int insideAllValid(const VBoundFrustum& bf) const;

	int insideAllValidFast(const VBoundSphere &bs) const;
	int insideAllValidFast(const VBoundBox &bb) const;
	int insideAllValidFast(const VBoundFrustum& bf) const;

	// Inside Planes bounds
	int insidePlanes(const VBoundSphere &bs) const;
	int insidePlanes(const VBoundBox &bb) const;
	int insidePlanes(const VBoundFrustum& bf) const;

	int insidePlanesValid(const VBoundSphere &bs) const;
	int insidePlanesValid(const VBoundBox &bb) const;
	int insidePlanesValid(const VBoundFrustum& bf) const;

	int insidePlanesValidFast(const VBoundSphere &bs) const;
	int insidePlanesValidFast(const VBoundBox &bb) const;
	int insidePlanesValidFast(const VBoundFrustum& bf) const;

	// Inside Portals
	int insidePortals(const VBoundSphere &bs) const;
	int insidePortals(const VBoundBox &bb) const;
	int insidePortals(const VBoundFrustum& bf) const;

	int insidePortalsValid(const VBoundSphere &bs) const;
	int insidePortalsValid(const VBoundBox &bb) const;
	int insidePortalsValid(const VBoundFrustum& bf) const;

	// Inside shadow
	bool InsideShadow(const VVector3& LightDirection, const VBoundSphere& BoundSphere) const;
	int insideShadow(const VBoundSphere &light,const VVector3& offset,const VBoundSphere &object) const;

	// parameters
	inline int isValid() const { return valid; }
	inline const VVector3& GetCameraPosition() const { return CameraPosition; }
	inline const VVector4* getPlanes() const { return Planes; }
	inline const VVector3* getPoints() const { return Points; }

	enum Corner
	{
		NEAR_LEFT_BOTTOM = 0,
		NEAR_RIGHT_BOTTOM,
		NEAR_LEFT_TOP,
		NEAR_RIGHT_TOP,
		FAR_LEFT_BOTTOM,
		FAR_RIGHT_BOTTOM,
		FAR_LEFT_TOP,
		FAR_RIGHT_TOP,
		CORNER_MAX,
	};

	/// 获取视锥的8个顶点. see enum Corner
	inline const VVector3* GetCorners() const { return Points; }
	inline const VVector3& GetCorner(UINT Index) { return Points[Index];}
	private:
	enum {
		PLANE_L = 0,
		PLANE_R,
		PLANE_B,
		PLANE_T,
		PLANE_N,
		PLANE_F,
	};
	struct Portal;
	int InsidePlanes(const VVector3& point) const;
	bool InsidePlanes(const VVector3& point,float radius) const;
	int InsidePlanes(const VVector3& min,const VVector3& max) const;
	int InsidePlanes(const VVector3 *Points,int num_points) const;


	int inside_planes_fast(const VVector3& point) const;

	/// 检查包围球是否在Frustum平面内部,忽略Near Far平面. 
	bool InsidePlanesFast(const VVector3& point,float radius) const;
	bool InsidePlanesFast(const VVector3& min,const VVector3& max, int NumPlanes = 4) const;
	

	int inside_planes_fast(const VVector3 *Points,int num_points) const;

	bool InsidePortal(const Portal& portal,const VVector3& point) const;
	bool InsidePortal(const Portal& portal,const VVector3& point,float radius) const;
	bool InsidePortal(const Portal& portal,const VVector3& min,const VVector3& max) const;
	bool InsidePortal(const Portal& portal,const VVector3 *Points,int num_points) const;

	inline bool InsidePortals(const VVector3& point) const
	{
		for(UINT i = 0; i < Portals.Size(); i++) 
		{
			if(InsidePortal(Portals[i],point) == 0) return false;
		}
		return true;
	}
	int InsidePortals(const VVector3& point,float radius) const;
	int InsidePortals(const VVector3& min,const VVector3& max) const;
	int InsidePortals(const VVector3* Points,int num_points) const;

	struct Portal {
		VVector4 Plane;					// portal Plane
		VVector4 Planes[4];				// aos clipping Planes
#if V_SSE || V_NEON
		VVector4 TPlanes[4];			// for fast SIMD math 
#endif 
		VVector3 Points[4];				// bounding Points
	};

	int valid;							// valid flag
	VVector3 CameraPosition;			// camera position. used with portal. 
	VVector4 Planes[6];					// aos clipping Planes
#if V_SSE || V_NEON
	VVector4 TPlanes[8];				// for fast SIMD math 
#endif 
	VVector3 Points[8];					// bounding Points
	VArray<Portal> Portals;
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
inline void VBoundBox::SetInclude(const VBoundSphere& Sphere)
{
	bool SphereValid = Sphere.IsValid();
	if (SphereValid && IsValidFast())
	{
		VVector3 RadiusVec(Sphere.Radius);
		VVector3 Min1 = Sphere.Center - RadiusVec;
		VVector3 Max1 = Sphere.Center + RadiusVec;
		Min.x = VMin(Min.x, Min1.x);
		Min.y = VMin(Min.y, Min1.y);
		Min.z = VMin(Min.z, Min1.z);
		Max.x = VMin(Max.x, Max1.x);
		Max.y = VMin(Max.y, Max1.y);
		Max.z = VMin(Max.z, Max1.z);
	}else if (SphereValid)
	{
		Min = Sphere.Center - VVec3_EPS ; Max = Sphere.Center + VVec3_EPS;
	}
}

inline void VBoundBox::SetInclude(const VBound& Bound)
{
	SetInclude(Bound.GetBoundBox());
}

inline bool VBoundBox::Intersect(const VBoundSphere& Sphere) const
{
	// Note: 快速比较. 并不精确. 如果要精确判断的话, 我们需要计算push的轴和距离. 
	if(Min.x > Sphere.Center.x + Sphere.Radius || Max.x < Sphere.Center.x - Sphere.Radius) return 0;
	if(Min.y > Sphere.Center.y + Sphere.Radius || Max.y < Sphere.Center.y - Sphere.Radius) return 0;
	if(Min.z > Sphere.Center.z + Sphere.Radius || Max.z < Sphere.Center.z - Sphere.Radius) return 0;
	return 1;
}

inline bool VBoundBox::Intersect(const VBound& Bound) const
{
	return Intersect(Bound.GetBoundBox());
}

inline EVOverlapResult VBoundBox::OverlapCheck(const VBoundBox& Box) const
{
	if(Min.x > Box.Max.x || Max.x < Box.Min.x) return VOR_OUTSIDE;
	if(Min.y > Box.Max.y || Max.y < Box.Min.y) return VOR_OUTSIDE;
	if(Min.z > Box.Max.z || Max.z < Box.Min.z) return VOR_OUTSIDE;
	if (Box.Min.x > Min.x && Box.Max.x < Max.x &&
		Box.Min.y > Min.y && Box.Max.y < Max.y &&
		Box.Min.z > Min.z && Box.Max.z < Max.z)
	{
		return VOR_INSIDE;
	}
	return VOR_OVERLAP;
}


inline float VBoundBox::DistanceSqr(const VVector3& Point) const
{
	VVector3 Axis(0.0f);

	if(Min.x > Point.x) Axis.x = Min.x - Point.x;
	else if(Max.x < Point.x) Axis.x = Max.x - Point.x;
	

	if(Min.y > Point.y) Axis.y = Min.y - Point.y;
	else if(Max.y < Point.y) Axis.y = Max.y - Point.y;
	
	if(Min.z > Point.z) Axis.z = Min.z - Point.z;
	else if(Max.z < Point.z) Axis.z = Max.z - Point.z;
	
	return Axis.LengthSqr();
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

/*
*/
inline int VBoundFrustum::insideFast(const VVector3& point) const {
	if(InsidePortals(point) == 0) return 0;
	return inside_planes_fast(point);
}

inline int VBoundFrustum::insideFast(const VVector3& point,float radius) const {
	if(InsidePortals(point,radius) == 0) return 0;
//	return inside_planes_fast(point,radius);
}

inline int VBoundFrustum::insideFast(const VVector3& min,const VVector3& max) const {
	if(InsidePortals(min,max) == 0) return 0;
//	return inside_planes_fast(min,max);
}

inline int VBoundFrustum::insideFast(const VVector3 *Points,int num_points) const {
	if(InsidePortals(Points,num_points) == 0) return 0;
	return inside_planes_fast(Points,num_points);
}

/*
*/
inline int VBoundFrustum::insideValid(const VBoundSphere &bs) const {
	return Inside(bs.Center,bs.Radius);
}

inline int VBoundFrustum::insideValid(const VBoundBox &bb) const {
	return Inside(bb.Min,bb.Max);
}

inline int VBoundFrustum::insideValid(const VBoundFrustum& bf) const {
	return Inside(bf.Points,8);
}

/*
*/
inline int VBoundFrustum::insideValidFast(const VBoundSphere &bs) const {
	return insideFast(bs.Center,bs.Radius);
}

inline int VBoundFrustum::insideValidFast(const VBoundBox &bb) const {
	return insideFast(bb.Min,bb.Max);
}

inline int VBoundFrustum::insideValidFast(const VBoundFrustum& bf) const {
	return insideFast(bf.Points,8);
}

/*
*/
inline int VBoundFrustum::insideAllValid(const VBoundSphere &bs) const {
	return Inside(bs.Center,-bs.Radius);
}

inline int VBoundFrustum::insideAllValid(const VBoundBox &bb) const {
	const VVector3& min = bb.Min;
	const VVector3& max = bb.Max;
	if(Inside(VVector3(min.x,min.y,min.z)) == 0) return 0;
	if(Inside(VVector3(max.x,min.y,min.z)) == 0) return 0;
	if(Inside(VVector3(min.x,max.y,min.z)) == 0) return 0;
	if(Inside(VVector3(max.x,max.y,min.z)) == 0) return 0;
	if(Inside(VVector3(min.x,min.y,max.z)) == 0) return 0;
	if(Inside(VVector3(max.x,min.y,max.z)) == 0) return 0;
	if(Inside(VVector3(min.x,max.y,max.z)) == 0) return 0;
	if(Inside(VVector3(max.x,max.y,max.z)) == 0) return 0;
	return 1;
}

inline int VBoundFrustum::insideAllValid(const VBoundFrustum& bf) const {
	for(int i = 0; i < 8; i++) {
		if(Inside(bf.Points[i]) == 0) return 0;
	}
	return 1;
}

/*
*/
inline int VBoundFrustum::insideAllValidFast(const VBoundSphere &bs) const {
	return insideFast(bs.Center,-bs.Radius);
}

inline int VBoundFrustum::insideAllValidFast(const VBoundBox &bb) const {
	const VVector3& min = bb.Min;
	const VVector3& max = bb.Max;
	if(insideFast(VVector3(min.x,min.y,min.z)) == 0) return 0;
	if(insideFast(VVector3(max.x,min.y,min.z)) == 0) return 0;
	if(insideFast(VVector3(min.x,max.y,min.z)) == 0) return 0;
	if(insideFast(VVector3(max.x,max.y,min.z)) == 0) return 0;
	if(insideFast(VVector3(min.x,min.y,max.z)) == 0) return 0;
	if(insideFast(VVector3(max.x,min.y,max.z)) == 0) return 0;
	if(insideFast(VVector3(min.x,max.y,max.z)) == 0) return 0;
	if(insideFast(VVector3(max.x,max.y,max.z)) == 0) return 0;
	return 1;
}

inline int VBoundFrustum::insideAllValidFast(const VBoundFrustum& bf) const {
	for(int i = 0; i < 8; i++) {
		if(insideFast(bf.Points[i]) == 0) return 0;
	}
	return 1;
}

/*
*/
inline int VBoundFrustum::insidePlanesValid(const VBoundSphere &bs) const {
	return InsidePlanes(bs.Center,bs.Radius);
}

inline int VBoundFrustum::insidePlanesValid(const VBoundBox &bb) const {
	return InsidePlanes(bb.Min,bb.Max);
}

inline int VBoundFrustum::insidePlanesValid(const VBoundFrustum& bf) const {
	return InsidePlanes(bf.Points,8);
}

/*
*/
inline int VBoundFrustum::insidePlanesValidFast(const VBoundSphere &bs) const {
//	return inside_planes_fast(bs.Center,bs.Radius);
	return 0;
}

inline int VBoundFrustum::insidePlanesValidFast(const VBoundBox &bb) const {
//	return inside_planes_fast(bb.Min,bb.Max);
	return 0;
}

inline int VBoundFrustum::insidePlanesValidFast(const VBoundFrustum& bf) const {
	return inside_planes_fast(bf.Points,8);
}

/*
*/
inline int VBoundFrustum::insidePortalsValid(const VBoundSphere &bs) const {
	return InsidePortals(bs.Center,bs.Radius);
}

inline int VBoundFrustum::insidePortalsValid(const VBoundBox &bb) const {
	return InsidePortals(bb.Min,bb.Max);
}

inline int VBoundFrustum::insidePortalsValid(const VBoundFrustum& bf) const {
	return InsidePortals(bf.Points,8);
}

#pragma warning(pop)

#endif 
