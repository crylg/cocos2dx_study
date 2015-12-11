#ifndef __VGEOMETRY_H__
#define __VGEOMETRY_H__
#pragma once
#include "VMath.h"
#include "VBounds.h"

// some common alogrithm for computational geometry. 

// point - point distance
inline float VDistanceSqr(const VVector3& Point1, const VVector3& Point2)
{
	return (Point1 - Point2).LengthSqr();
}

// point - plane distance
inline float VDistanceSqr(const VVector3& Point, const VVector4& Plane)
{
	return 0.0f;
}

// point - box distance
inline float VDistanceSqr(const VVector3& Point, const VBoundBox& Box);


inline int VOverlap(const VBoundBox& Box1, const VBoundBox& Box2);




//////////////////////////////////////////////////////////////////////////
//	Intersection
//////////////////////////////////////////////////////////////////////////

/// 判断射线与包围盒是否相交
/// Point : 射线起点
/// Direction : 射线方向
/// Box: 判断相交的包围盒
/// IntersectPoint : 交点. 可为空. 如果不为空且射线与包围盒相交, 这里返回交点. 
/// 如果射线与包围盒相交, 返回true. 否则返回false. 
VCoreApi bool VIntersectRayBox(const VVector3& Point, const VVector3& Direction, const VBoundBox& Box, VVector3* IntersectPoint = NULL); 

class VCGOverlap
{
public:

};


class VCGIntersect
{
public:

};










//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
inline bool VBoundBox::RayIntersect(const VVector3& Point, const VVector3& Direction) const
{
	return VIntersectRayBox(Point, Direction, *this, NULL);
}

#endif 
