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

/// �ж��������Χ���Ƿ��ཻ
/// Point : �������
/// Direction : ���߷���
/// Box: �ж��ཻ�İ�Χ��
/// IntersectPoint : ����. ��Ϊ��. �����Ϊ�����������Χ���ཻ, ���ﷵ�ؽ���. 
/// ����������Χ���ཻ, ����true. ���򷵻�false. 
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
