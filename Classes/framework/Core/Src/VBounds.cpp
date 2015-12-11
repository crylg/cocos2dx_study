#include "VStdAfx.h"

#if VPLATFORM_WIN32 // NOTE : for 2d . 

#include "VBounds.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void VBoundSphere::SetTight(const VVector3* Points, int NumPoints)
{
	VASSERT(NumPoints > 2);
	Radius = VINFINITY;

	// 紧凑包围球一定具备一个特征: 中心点在某两个点的连线中点上.
	float RadiusTest;
	VVector3 CenterTest;

	for(int Index = 0; Index < NumPoints; ++Index) 
	{
		for(int NextIndex = Index + 1; NextIndex < NumPoints; NextIndex++) 
		{
			RadiusTest = -VINFINITY;
			// 假设为中心
			CenterTest = (Points[Index] + Points[NextIndex]) * 0.5f;

			// 遍历所有点与中心的距离.
			for(int k = 0; k < NumPoints; k++) 
			{
				// 计算最大距离, 作为半径
				float Dist = (Points[k] - CenterTest).LengthSqr();
				if(RadiusTest < Dist) RadiusTest = Dist;
			}

			// 如果当前测试点的半径比以前的更小, 选定这个点.
			if(Radius > RadiusTest) 
			{
				Center = CenterTest;
				Radius = RadiusTest;
			}
		}
	}

	if (Radius > VEPS)
	{
		Radius = VSqrt(Radius);
	}else
	{
		Radius = 0.0f;
	}
}



//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

// TODO : 优化内存. Planes TPlanes . 在开启SIMD 的时候应该可以只需要TPlanes 否则只需要Planes ???
VBoundFrustum::VBoundFrustum() : valid(0) 
{
	
}

VBoundFrustum::VBoundFrustum(const VMatrix& projection,const VMatrix& modelview) 
{
	Set(projection,modelview);
}

VBoundFrustum::VBoundFrustum(const VBoundFrustum& bf)
	:CameraPosition(bf.CameraPosition)
{
	// clipping Planes and Points
	for(int i = 0; i < 6; i++) 
	{
		Planes[i] = bf.Planes[i];
	}
	for(int i = 0; i < 8; i++) 
	{
#if V_SSE || V_NEON
		TPlanes[i] = bf.TPlanes[i];
#endif 
		Points[i] = bf.Points[i];
	}
	Portals = bf.Portals;
}


VBoundFrustum::~VBoundFrustum() 
{
	
}

void VBoundFrustum::Set(const VMatrix& ModelView, const VMatrix& Projection) 
{
	VMatrix MVP = ModelView * Projection;

	// CameraPosition
	VMatrix InvModeView = ModelView.Inverse();
	CameraPosition = InvModeView.GetRow(3);

	//CameraPosition =  inverse(modelview).getColumn3(3);
	
	// Points
	//Points[0].Set(-1.0f,-1.0f,-1.0f);	// set z = 0.0f ?????
	//Points[1].Set( 1.0f,-1.0f,-1.0f);
	//Points[2].Set(-1.0f, 1.0f,-1.0f);
	//Points[3].Set( 1.0f, 1.0f,-1.0f);
	Points[0].Set(-1.0f,-1.0f,0.0f);	// set z = 0.0f ?????
	Points[1].Set( 1.0f,-1.0f,0.0f);
	Points[2].Set(-1.0f, 1.0f,0.0f);
	Points[3].Set( 1.0f, 1.0f,0.0f);

	Points[4].Set(-1.0f,-1.0f, 1.0f);	// set
	Points[5].Set( 1.0f,-1.0f, 1.0f);
	Points[6].Set(-1.0f, 1.0f, 1.0f);
	Points[7].Set( 1.0f, 1.0f, 1.0f);
	
	// transform to world pos.
	VMatrix InvMVP = MVP.Inverse();
	for (int Index = 0; Index < 8; ++Index)
	{
		// InvMVP.TransformVec3(Points[Index],Points[Index]);
		 InvMVP.TransformVec3Projection(Points[Index],Points[Index]);
	}
	
	//calc clipping Planes
	Planes[PLANE_L].Set(MVP.m14 + MVP.m11,MVP.m24 + MVP.m21,MVP.m34 + MVP.m31,MVP.m44 + MVP.m41);
	Planes[PLANE_R].Set(MVP.m14 - MVP.m11,MVP.m24 - MVP.m21,MVP.m34 - MVP.m31,MVP.m44 - MVP.m41);
	Planes[PLANE_B].Set(MVP.m14 + MVP.m12,MVP.m24 + MVP.m22,MVP.m34 + MVP.m32,MVP.m44 + MVP.m42);
	Planes[PLANE_T].Set(MVP.m14 - MVP.m12,MVP.m24 - MVP.m22,MVP.m34 - MVP.m32,MVP.m44 - MVP.m42);
	//Planes[PLANE_N].Set(MVP.m14 + MVP.m13,MVP.m24 + MVP.m23,MVP.m34 + MVP.m33,MVP.m44 + MVP.m43);
	Planes[PLANE_N].Set(          MVP.m13,          MVP.m23,          MVP.m33,          MVP.m43);
	Planes[PLANE_F].Set(MVP.m14 - MVP.m13,MVP.m24 - MVP.m23,MVP.m34 - MVP.m33,MVP.m44 - MVP.m43);
	for(int i = 0; i < 6; i++) 
	{
		Planes[i].Normalize();
	}

#if V_SSE || V_NEON
	// transform planes [4X4]
	for(int i = 0; i < 4; i++) 
	{
		TPlanes[0][i] = Planes[i].x;
		TPlanes[1][i] = Planes[i].y;
		TPlanes[2][i] = Planes[i].z;
		TPlanes[3][i] = Planes[i].w;
	}
	for(int i = 0, j = 4; i < 2; i++, j++) 
	{
		TPlanes[4][i] = Planes[j].x;
		TPlanes[5][i] = Planes[j].y;
		TPlanes[6][i] = Planes[j].z;
		TPlanes[7][i] = Planes[j].w;
	}
#endif 
	// Portals
	Portals.Clear();
}


EVOverlapResult VBoundFrustum::OverlapCheckPlane(const VBoundSphere& Sphere) const
{
	EVOverlapResult Result = VOR_OUTSIDE;
	
#if V_SSE
	BOOL FullInside = TRUE;

	__m128 Radius = _mm_load1_ps((float*)&Sphere.Radius);
	__m128 NegRadius = _mm_sub_ps(_mm_setzero_ps(),Radius);

	__m128 CenterX = _mm_set1_ps(Sphere.Center.x);
	__m128 CenterY = _mm_set1_ps(Sphere.Center.y);
	__m128 CenterZ = _mm_set1_ps(Sphere.Center.z);

	__m128 res_0 = _mm_mul_ps(TPlanes[0].vec,CenterX);		// [p0.a p1.a p2.a p3.a] * [x,x,x,x]   
	__m128 res_1 = _mm_mul_ps(TPlanes[1].vec,CenterY);		// [p0.b p1.b p2.b p3.b] * [y,y,y,y]
	__m128 res_2 = _mm_mul_ps(TPlanes[2].vec,CenterZ);		// [p0.c p1.c p2.c p3.c] * [z,z,z,z]
	__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[3].vec));	// Center - plane distantc [d0,d1,d2,d3]

	int Mask = _mm_movemask_ps(_mm_cmplt_ps(res_3,NegRadius)) & 0x0f;
	if (Mask != 0)
	{
		// 在上面4个面的检测中, 包围球完全落入某个面的外部.
		return VOR_OUTSIDE;
	}
	
	// 至少有 1个面, 球心位于其前面. 
	// 检查是否处于4个面的前面(全包含)
	Mask = _mm_movemask_ps(_mm_cmplt_ps(res_3,Radius)) & 0x0f;
	if (Mask)
	{
		// 球于某个面相交. 
		FullInside = FALSE;
	}
	
	// near and far plane
	res_0 = _mm_mul_ps(TPlanes[4].vec,CenterX);	// [p4.a p5.a U U] * [x,x,x,x]
	res_1 = _mm_mul_ps(TPlanes[5].vec,CenterY);	// [p4.b p5.b U U] * [y,y,y,y]
	res_2 = _mm_mul_ps(TPlanes[6].vec,CenterZ);	// [p4.c p5.c U U] * [z,z,z,z]
	res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[7].vec));	// // Center - plane distantc [d4,d5,U,U]
	Mask = _mm_movemask_ps(_mm_cmplt_ps(res_3,NegRadius)) & 0x0f;
	if (Mask != 0)
	{
		// 球体完全落入N/F 的外部
		return VOR_OUTSIDE;
	}

	Mask = _mm_movemask_ps(_mm_cmplt_ps(res_3,Radius)) & 0x0f;
	if (Mask)
	{
		// 球于N/F相交. 
		FullInside = FALSE;
	}

	return FullInside ? VOR_INSIDE : VOR_OVERLAP;

#else

#endif 

	return VOR_OUTSIDE;
}

EVOverlapResult VBoundFrustum::OverlapCheckPortal(const VBoundSphere& Sphere) const
{
	return VOR_OUTSIDE;
}

// Overlap Check with frustum plane
EVOverlapResult VBoundFrustum::OverlapCheckPlane(const VBoundBox& Box) const
{
#if V_SSE
	BOOL FullInside = TRUE;
	VVector3 Temp = (Box.Min + Box.Max) * 0.5f;
	__m128 CenterX = _mm_set1_ps(Temp.x);
	__m128 CenterY = _mm_set1_ps(Temp.y);
	__m128 CenterZ = _mm_set1_ps(Temp.z);
	Temp = (Box.Max - Box.Min) * 0.5f;		// Half Size
	__m128 SizeX = _mm_set1_ps(Temp.x);
	__m128 SizeY = _mm_set1_ps(Temp.y);
	__m128 SizeZ = _mm_set1_ps(Temp.z);

	VIntFloat SignValue(0x7fffffff);
	__m128 Sign = _mm_set1_ps(SignValue.f);
	//__m128 AbsSizeX = _mm_and_ps(SizeX, Sign);			// Always >0
	//__m128 AbsSizeY = _mm_and_ps(SizeY, Sign);
	//__m128 AbsSizeZ = _mm_and_ps(SizeZ, Sign);


	// 4 planes
	__m128 res_0 = _mm_mul_ps(TPlanes[0].vec,CenterX);		// [p0.a p1.a p2.a p3.a] * [x,x,x,x]   
	__m128 res_1 = _mm_mul_ps(TPlanes[1].vec,CenterY);		// [p0.b p1.b p2.b p3.b] * [y,y,y,y]
	__m128 res_2 = _mm_mul_ps(TPlanes[2].vec,CenterZ);		// [p0.c p1.c p2.c p3.c] * [z,z,z,z]
	__m128 Distance = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[3].vec));	// Center - plane distantc [d0,d1,d2,d3]

	// 求出在平面法线方向上. AABB的最大投影长度. 
	res_0 = _mm_and_ps(TPlanes[0].vec, Sign);			// abs
	res_0 = _mm_mul_ps(res_0,SizeX);		// [Abs(p0.a) Abs(p1.a) Abs(p2.a) Abs(p3.a)] * [sx,sx,sx,sx] 
	res_1 = _mm_and_ps(TPlanes[1].vec, Sign);			// abs
	res_1 = _mm_mul_ps(res_1,SizeY);		// [p0.b p1.b p2.b p3.b] * [y,y,y,y]
	res_2 = _mm_and_ps(TPlanes[2].vec, Sign);			// abs
	res_2 = _mm_mul_ps(res_2,SizeZ);		// [p0.c p1.c p2.c p3.c] * [z,z,z,z]
	__m128 Push = _mm_add_ps(_mm_add_ps(res_0,res_1),res_2);
	__m128 NegPush = _mm_sub_ps(_mm_setzero_ps(),Push);

	int Mask = _mm_movemask_ps(_mm_cmplt_ps(Distance,NegPush)) & 0x0f;
	if (Mask != 0)
	{
		// 在上面4个面的检测中, 包围盒完全落入某个面的外部.
		return VOR_OUTSIDE;
	}

	// 至少有 1个面, 球心位于其前面. 
	// 检查是否处于4个面的前面(全包含)
	Mask = _mm_movemask_ps(_mm_cmplt_ps(Distance,Push)) & 0x0f;
	if (Mask)
	{
		// 包围盒与某个面相交. 
		FullInside = FALSE;
	}

	// Near and far
	res_0 = _mm_mul_ps(TPlanes[4].vec,CenterX);		// [p0.a p1.a p2.a p3.a] * [x,x,x,x]   
	res_1 = _mm_mul_ps(TPlanes[5].vec,CenterY);		// [p0.b p1.b p2.b p3.b] * [y,y,y,y]
	res_2 = _mm_mul_ps(TPlanes[6].vec,CenterZ);		// [p0.c p1.c p2.c p3.c] * [z,z,z,z]
	Distance = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[7].vec));	// Center - plane distantc [d0,d1,d2,d3]

	// 求出在平面法线方向上. AABB的最大投影长度. 
	res_0 = _mm_and_ps(TPlanes[4].vec, Sign);			// abs
	res_0 = _mm_mul_ps(res_0,SizeX);		// [Abs(p0.a) Abs(p1.a) Abs(p2.a) Abs(p3.a)] * [sx,sx,sx,sx] 
	res_1 = _mm_and_ps(TPlanes[5].vec, Sign);			// abs
	res_1 = _mm_mul_ps(res_1,SizeY);		// [p0.b p1.b p2.b p3.b] * [y,y,y,y]
	res_2 = _mm_and_ps(TPlanes[6].vec, Sign);			// abs
	res_2 = _mm_mul_ps(res_2,SizeZ);		// [p0.c p1.c p2.c p3.c] * [z,z,z,z]
	Push = _mm_add_ps(_mm_add_ps(res_0,res_1),res_2);
	NegPush = _mm_sub_ps(_mm_setzero_ps(),Push);

	Mask = _mm_movemask_ps(_mm_cmplt_ps(Distance,NegPush)) & 0x0f;
	if (Mask != 0)
	{
		// 在上面4个面的检测中, 包围盒完全落入某个面的外部.
		return VOR_OUTSIDE;
	}

	// 至少有 1个面, 球心位于其前面. 
	// 检查是否处于4个面的前面(全包含)
	Mask = _mm_movemask_ps(_mm_cmplt_ps(Distance,Push)) & 0x0f;
	if (Mask)
	{
		// 包围盒与某个面相交. 
		FullInside = FALSE;
	}

	return FullInside ? VOR_INSIDE : VOR_OVERLAP;
#else 





	//http://zach.in.tu-clausthal.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/index.html
	//Geometric Approach - Testing Boxes II
	VVector3 Min(Box.Max),Max(Box.Min);

	if (Normal.x > 0.0f)
	{
		Max.x = Box.Max.x;
		Min.x = Box.Min.x;
	}
	if (Normal.y > 0.0f)
	{
		Max.y = Box.Max.y;
		Min.y = Box.Min.y;
	}
	if (Normal.z > 0.0f)
	{
		Max.z = Box.Max.z;
		Min.z = Box.Min.z;
	}
	int result = INSIDE;
	//for each plane do ...
	for(int i=0; i < 6; i++) {

		// is the positive vertex outside?
		if (pl[i].distance(b.getVertexP(pl[i].normal)) < 0)
			return OUTSIDE;
		// is the negative vertex outside?	
		else if (pl[i].distance(b.getVertexN(pl[i].normal)) < 0)
			result =  INTERSECT;
	}
	return(result);

	UBOOL Result = TRUE;

	// Assume fully contained
	bOutFullyContained = TRUE;

	checkSlow(PermutedPlanes.Num() % 4 == 0);

	// Load the origin & extent
	VectorRegister Orig = VectorLoadFloat3(&Origin);
	VectorRegister Ext = VectorLoadFloat3(&Extent);
	// Splat origin into 3 vectors
	VectorRegister OrigX = VectorReplicate(Orig, 0);
	VectorRegister OrigY = VectorReplicate(Orig, 1);
	VectorRegister OrigZ = VectorReplicate(Orig, 2);
	// Splat extent into 3 vectors
	VectorRegister ExtentX = VectorReplicate(Ext, 0);
	VectorRegister ExtentY = VectorReplicate(Ext, 1);
	VectorRegister ExtentZ = VectorReplicate(Ext, 2);
	// Splat the abs for the pushout calculation
	VectorRegister AbsExt = VectorAbs(Ext);
	VectorRegister AbsExtentX = VectorReplicate(AbsExt, 0);
	VectorRegister AbsExtentY = VectorReplicate(AbsExt, 1);
	VectorRegister AbsExtentZ = VectorReplicate(AbsExt, 2);
	// Since we are moving straight through get a pointer to the data
	const FPlane* RESTRICT PermutedPlanePtr = (FPlane*)PermutedPlanes.GetData();
	// Process four planes at a time until we have < 4 left
	for (INT Count = 0; Count < PermutedPlanes.Num(); Count += 4)
	{
		// Load 4 planes that are already all Xs, Ys, ...
		VectorRegister PlanesX = VectorLoadAligned(PermutedPlanePtr);
		PermutedPlanePtr++;
		VectorRegister PlanesY = VectorLoadAligned(PermutedPlanePtr);
		PermutedPlanePtr++;
		VectorRegister PlanesZ = VectorLoadAligned(PermutedPlanePtr);
		PermutedPlanePtr++;
		VectorRegister PlanesW = VectorLoadAligned(PermutedPlanePtr);
		PermutedPlanePtr++;
		// Calculate the distance (x * x) + (y * y) + (z * z) - w
		VectorRegister DistX = VectorMultiply(OrigX,PlanesX);
		VectorRegister DistY = VectorMultiplyAdd(OrigY,PlanesY,DistX);
		VectorRegister DistZ = VectorMultiplyAdd(OrigZ,PlanesZ,DistY);
		VectorRegister Distance = VectorSubtract(DistZ,PlanesW);
		// Now do the push out Abs(x * x) + Abs(y * y) + Abs(z * z)
		VectorRegister PushX = VectorMultiply(AbsExtentX,VectorAbs(PlanesX));
		VectorRegister PushY = VectorMultiplyAdd(AbsExtentY,VectorAbs(PlanesY),PushX);
		VectorRegister PushOut = VectorMultiplyAdd(AbsExtentZ,VectorAbs(PlanesZ),PushY);
		VectorRegister PushOutNegative = VectorNegate(PushOut);

		// Check for completely outside
		if (VectorAnyGreaterThan(Distance,PushOut))
		{
			Result = FALSE;
			bOutFullyContained = FALSE;
			break;
		}

		// Definitely inside frustums, but check to see if it's fully contained
		if (VectorAnyGreaterThan(Distance,PushOutNegative))
		{
			bOutFullyContained = FALSE;
		}
	}
	return Result;

#endif 
	return VOR_OUTSIDE;
}

// Overlap check with portal clip planes. 
// if there is no portal, return OUTSIDE. 
EVOverlapResult VBoundFrustum::OverlapCheckPortal(const VBoundBox& Box) const
{
	return VOR_OUTSIDE;
}


void VBoundFrustum::setITransform(const VMatrix& itransform) 
{
	//VMatrix transform = inverse(itransform);
	//
	//// CameraPosition
	//CameraPosition = transform * CameraPosition;
	//
	//// Points
	//Simd::mulMat4Vec3(Points,sizeof(VVector3),transform,Points,sizeof(VVector3),8);
	//
	//// clipping Planes
	//for(int i = 0; i < 6; i++) {
	//	Planes[i] = Planes[i] * itransform;
	//	Planes[i] /= length(VVector3(Planes[i]));
	//}
	//for(int i = 0; i < 4; i++) {
	//	TPlanes[0][i] = Planes[i].x;
	//	TPlanes[1][i] = Planes[i].y;
	//	TPlanes[2][i] = Planes[i].z;
	//	TPlanes[3][i] = Planes[i].w;
	//}
	//for(int i = 0, j = 4; i < 2; i++, j++) {
	//	TPlanes[4][i] = Planes[j].x;
	//	TPlanes[5][i] = Planes[j].y;
	//	TPlanes[6][i] = Planes[j].z;
	//	TPlanes[7][i] = Planes[j].w;
	//}
	//
	//// Portals
	//for(int i = 0; i < Portals.size(); i++) {
	//	Portal& portal = Portals[i];
	//	portal.Plane = portal.Plane * itransform;
	//	portal.Plane /= length(VVector3(portal.Plane));
	//	Simd::mulMat4Vec3(portal.Points,sizeof(VVector3),transform,portal.Points,sizeof(VVector3),4);
	//	for(int j = 0; j < 4; j++) {
	//		portal.Planes[j] = portal.Planes[j] * itransform;
	//		portal.Planes[j] /= length(VVector3(portal.Planes[j]));
	//		portal.TPlanes[0][j] = portal.Planes[j].x;
	//		portal.TPlanes[1][j] = portal.Planes[j].y;
	//		portal.TPlanes[2][j] = portal.Planes[j].z;
	//		portal.TPlanes[3][j] = portal.Planes[j].w;
	//	}
	//}
}

void VBoundFrustum::AddPortal(const VVector3 *Points,int num_points,const VMatrix& transform) 
{
	// TODO : 当前我们不支持任意多边形的Portal.... 
	// 原因是因为我们需要平面按照4的倍数增加时候, 我们可以使用transformed plane 进行SIMD优化. 
	VASSERT(num_points == 4 && "VBoundFrustum::AddPortal(): bad Points number");
	
	Portal portal;
	
	// portal Points
	for(int i = 0; i < num_points; i++) 
	{
		transform.TransformVec3(portal.Points[i], Points[i]);
	}
	
	// check visibility
	if(!Inside(portal.Points,num_points)) 
	{
		return;
	}
	
	
	VVector3 Edge0 = portal.Points[1] - portal.Points[0];
	VVector3 Edge1 = portal.Points[2] - portal.Points[0];
	VVector3 Normal = Edge0.Cross(Edge1);

	// Normalize.
	float Len = Normal.Length();
	if (Len < VEPS)
	{
		return;
	}
	Normal /= Len;

	
	// portal Plane
	Edge0 = CameraPosition - portal.Points[0];
	float angle = Edge0.Dot(Normal);
	if (angle > 0.0f)
	{
		Normal = -Normal;
	}
	portal.Plane.Set(Normal, -Normal.Dot(portal.Points[0]));
	
	// clipping Planes
	int j = (angle > 0.0f) ? num_points - 1 : 1;
	for(int i = 0; i < num_points; i++) 
	{
		Edge0 = portal.Points[i] - CameraPosition;
		Edge1 = portal.Points[j] - CameraPosition;
		Normal = Edge0.Cross(Edge1);
		Normal.Normalize();
		portal.Planes[i].Set(Normal, -Normal.Dot(CameraPosition));
		if(++j == num_points) j = 0;
	}
	
#if V_SSE || V_NEON
	// copy Planes
	for(int i = 0; i < 4; i++) {
		portal.TPlanes[0][i] = portal.Planes[i].x;
		portal.TPlanes[1][i] = portal.Planes[i].y;
		portal.TPlanes[2][i] = portal.Planes[i].z;
		portal.TPlanes[3][i] = portal.Planes[i].w;
	}
#endif 
	// add portal
	Portals.PushBack(portal);
}

/*
 */
void VBoundFrustum::expand(float radius) {
	
	// clipping Planes
	for(int i = 0; i < 6; i++) {
		Planes[i].w += radius;
	}
	for(int i = 0; i < 4; i++) {
		TPlanes[0][i] = Planes[i].x;
		TPlanes[1][i] = Planes[i].y;
		TPlanes[2][i] = Planes[i].z;
		TPlanes[3][i] = Planes[i].w;
	}
	for(int i = 0, j = 4; i < 2; i++, j++) {
		TPlanes[4][i] = Planes[j].x;
		TPlanes[5][i] = Planes[j].y;
		TPlanes[6][i] = Planes[j].z;
		TPlanes[7][i] = Planes[j].w;
	}
}


static inline bool inside_plane(const VVector4& Plane,const VVector3& Min,const VVector3& Max) 
{
	// SAT
	VVector3 Center = (Min + Max) * 0.5f;
	VVector3 Size = (Max - Min)*0.5f;
	
	VVector3 Normal(Plane);
	Normal.x = VAbs(Normal.x);
	Normal.y = VAbs(Normal.y);
	Normal.z = VAbs(Normal.z);

	float Push = Normal.Dot(Size);

	float Distance = Plane.Dot(Center);

	if (Distance < -Push)
	{
		return false;
	}
	return true;


	/*

	#ifdef V_SSE
		__m128 min_xyz = _mm_loadu_ps((const float*)&min);
		__m128 max_xyz = _mm_loadu_ps((const float*)&max);
		min_xyz = _mm_mul_ps(min_xyz,Plane.vec);
		max_xyz = _mm_mul_ps(max_xyz,Plane.vec);
		__m128 min_max_x = _mm_shuffle_ps(min_xyz,max_xyz,VSHUFFLE_MASK(X,X,X,X));
		__m128 min_max_y = _mm_shuffle_ps(min_xyz,max_xyz,VSHUFFLE_MASK(Y,Y,Y,Y));
		min_max_x = _MM_SWIZZLE(min_max_x,X,Z,X,Z);
		min_max_y = _mm_add_ps(min_max_y,_MM_SWIZZLE(Plane.vec,W,W,W,W));
		__m128 min_max_xy = _mm_add_ps(min_max_x,min_max_y);
		__m128 res_0 = _mm_add_ps(min_max_xy,_MM_SWIZZLE(min_xyz,Z,Z,Z,Z));
		__m128 res_1 = _mm_add_ps(min_max_xy,_MM_SWIZZLE(max_xyz,Z,Z,Z,Z));
		if(_mm_movemask_ps(_mm_and_ps(res_0,res_1)) != 0x0f) return 1;
	#elif USE_ALTIVEC
		vec_float4 plane_w = vec_perm(vec_splats(0.0f),Plane.vec,VEC_PERM4(LX,LX,RW,LX));
		vec_float4 min_xyz = vec_madd(min.vec,Plane.vec,plane_w);
		vec_float4 max_xyz = vec_madd(max.vec,Plane.vec,plane_w);
		vec_float4 min_max_x = vec_perm(min_xyz,max_xyz,VEC_PERM4(LX,RX,LX,RX));
		vec_float4 min_max_y = vec_perm(min_xyz,max_xyz,VEC_PERM4(LY,LY,RY,RY));
		vec_float4 min_max_xy = vec_add(min_max_x,min_max_y);
		vec_float4 res_0 = vec_add(min_max_xy,VEC_SWIZZLE(min_xyz,Z,Z,Z,Z));
		vec_float4 res_1 = vec_add(min_max_xy,VEC_SWIZZLE(max_xyz,Z,Z,Z,Z));
		vec_uint4 res_2 = (vec_uint4)vec_and(res_0,res_1);
		if((VEC_SWIZZLE(res_2,B0,B0,B0,B0)[0] & 0x80808080) != 0x80808080) return 1;
	#else
		float min_x = min.x * Plane.x;
		float min_y = min.y * Plane.y;
		float min_zw = min.z * Plane.z + Plane.w;
		float min_min_xy = min_x + min_y;
		if(min_min_xy + min_zw > 0.0f) return 1;
		float max_x = max.x * Plane.x;
		float max_min_xy = max_x + min_y;
		if(max_min_xy + min_zw > 0.0f) return 1;
		float max_y = max.y * Plane.y;
		float min_max_xy = min_x + max_y;
		if(min_max_xy + min_zw > 0.0f) return 1;
		float max_max_xy = max_x + max_y;
		if(max_max_xy + min_zw > 0.0f) return 1;
		float max_zw = max.z * Plane.z + Plane.w;
		if(min_min_xy + max_zw > 0.0f) return 1;
		if(max_min_xy + max_zw > 0.0f) return 1;
		if(min_max_xy + max_zw > 0.0f) return 1;
		if(max_max_xy + max_zw > 0.0f) return 1;
	#endif
	return 0;*/
}

// 是否有某个点处于面的正面???
static inline bool inside_plane(const VVector4& Plane,const VVector3* Points,int num_points) 
{
	for(int i = 0; i < num_points; i++) 
	{
		if(Plane.Dot(Points[i]) > 0.0f)
		{
			return true;
		}
	}
	return false;
}


int VBoundFrustum::InsidePlanes(const VVector3& point) const 
{
	#if V_SSE
		__m128 res_0 = _mm_mul_ps(TPlanes[0].vec,_mm_set1_ps(point.x));
		__m128 res_1 = _mm_mul_ps(TPlanes[1].vec,_mm_set1_ps(point.y));
		__m128 res_2 = _mm_mul_ps(TPlanes[2].vec,_mm_set1_ps(point.z));
		__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[3].vec));
		if(_mm_movemask_ps(res_3) & 0x0f) 
			return 0;
		res_0 = _mm_mul_ps(TPlanes[4].vec,_mm_set1_ps(point.x));
		res_1 = _mm_mul_ps(TPlanes[5].vec,_mm_set1_ps(point.y));
		res_2 = _mm_mul_ps(TPlanes[6].vec,_mm_set1_ps(point.z));
		res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[7].vec));
		if(_mm_movemask_ps(res_3) & 0x03) 
			return 0;
	#elif USE_ALTIVEC
		vec_float4 res_0 = vec_madd(TPlanes[0].vec,vec_splats(point.x),TPlanes[3].vec);
		vec_float4 res_1 = vec_madd(TPlanes[1].vec,vec_splats(point.y),res_0);
		vec_float4 res_2 = vec_madd(TPlanes[2].vec,vec_splats(point.z),res_1);
		if(VEC_SWIZZLE((vec_uint4)res_2,B0,B0,B0,B0)[0] & 0x80808080) return 0;
		res_0 = vec_madd(TPlanes[4].vec,vec_splats(point.x),TPlanes[7].vec);
		res_1 = vec_madd(TPlanes[5].vec,vec_splats(point.y),res_0);
		res_2 = vec_madd(TPlanes[6].vec,vec_splats(point.z),res_1);
		if(VEC_SWIZZLE((vec_uint4)res_2,B0,B0,B0,B0)[0] & 0x80800000) return 0;
	#elif V_NEON
		float32x4_t res_0 = vmlaq_n_f32(TPlanes[3].vec,TPlanes[0].vec,point.x);
		float32x4_t res_1 = vmlaq_n_f32(res_0,TPlanes[1].vec,point.y);
		float32x4_t res_2 = vmlaq_n_f32(res_1,TPlanes[2].vec,point.z);
		if(vmaskq_u32(vcltq_f32(res_2,vdupq_n_f32(0.0f)))) return 0;
		float32x2_t res_3 = vmla_n_f32(vget_low_f32(TPlanes[7].vec),vget_low_f32(TPlanes[4].vec),point.x);
		float32x2_t res_4 = vmla_n_f32(res_3,vget_low_f32(TPlanes[5].vec),point.y);
		float32x2_t res_5 = vmla_n_f32(res_4,vget_low_f32(TPlanes[6].vec),point.z);
		if(vmask_u32(vclt_f32(res_5,vdup_n_f32(0.0f)))) return 0;
	#else
		if(dot(Planes[0],point) < 0.0f) return 0;
		if(dot(Planes[1],point) < 0.0f) return 0;
		if(dot(Planes[2],point) < 0.0f) return 0;
		if(dot(Planes[3],point) < 0.0f) return 0;
		if(dot(Planes[4],point) < 0.0f) return 0;
		if(dot(Planes[5],point) < 0.0f) return 0;
	#endif
	return 1;
}

bool VBoundFrustum::InsidePlanes(const VVector3& point,float radius) const 
{
	#ifdef V_SSE
		__m128 res_0 = _mm_mul_ps(TPlanes[0].vec,_mm_set1_ps(point.x));
		__m128 res_1 = _mm_mul_ps(TPlanes[1].vec,_mm_set1_ps(point.y));
		__m128 res_2 = _mm_mul_ps(TPlanes[2].vec,_mm_set1_ps(point.z));
		__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[3].vec));
		if(_mm_movemask_ps(_mm_add_ps(res_3,_mm_set1_ps(radius))) & 0x0f) return false;
		res_0 = _mm_mul_ps(TPlanes[4].vec,_mm_set1_ps(point.x));
		res_1 = _mm_mul_ps(TPlanes[5].vec,_mm_set1_ps(point.y));
		res_2 = _mm_mul_ps(TPlanes[6].vec,_mm_set1_ps(point.z));
		res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[7].vec));
		if(_mm_movemask_ps(_mm_add_ps(res_3,_mm_set1_ps(radius))) & 0x03) return false;
	#elif USE_ALTIVEC
		vec_float4 res_0 = vec_madd(TPlanes[0].vec,vec_splats(point.x),TPlanes[3].vec);
		vec_float4 res_1 = vec_madd(TPlanes[1].vec,vec_splats(point.y),res_0);
		vec_float4 res_2 = vec_madd(TPlanes[2].vec,vec_splats(point.z),res_1);
		vec_uint4 res_3 = (vec_uint4)vec_add(res_2,vec_splats(radius));
		if(VEC_SWIZZLE(res_3,B0,B0,B0,B0)[0] & 0x80808080) return 0;
		res_0 = vec_madd(TPlanes[4].vec,vec_splats(point.x),TPlanes[7].vec);
		res_1 = vec_madd(TPlanes[5].vec,vec_splats(point.y),res_0);
		res_2 = vec_madd(TPlanes[6].vec,vec_splats(point.z),res_1);
		res_3 = (vec_uint4)vec_add(res_2,vec_splats(radius));
		if(VEC_SWIZZLE(res_3,B0,B0,B0,B0)[0] & 0x80800000) return 0;
	#elif USE_NEON
		float32x4_t res_0 = vmlaq_n_f32(TPlanes[3].vec,TPlanes[0].vec,point.x);
		float32x4_t res_1 = vmlaq_n_f32(res_0,TPlanes[1].vec,point.y);
		float32x4_t res_2 = vmlaq_n_f32(res_1,TPlanes[2].vec,point.z);
		if(vmaskq_u32(vcltq_f32(res_2,vdupq_n_f32(-radius)))) return 0;
		float32x2_t res_3 = vmla_n_f32(vget_low_f32(TPlanes[7].vec),vget_low_f32(TPlanes[4].vec),point.x);
		float32x2_t res_4 = vmla_n_f32(res_3,vget_low_f32(TPlanes[5].vec),point.y);
		float32x2_t res_5 = vmla_n_f32(res_4,vget_low_f32(TPlanes[6].vec),point.z);
		if(vmask_u32(vclt_f32(res_5,vdup_n_f32(-radius)))) return 0;
	#else
	for (int i = 0; i < 6; ++i)
	{
		if (Planes[i].Dot(point) < -radius)
		{
			return false;
		}
	}
	return true;
	#endif
	return true;
}

int VBoundFrustum::InsidePlanes(const VVector3& min,const VVector3& max) const {
	if(inside_plane(Planes[0],min,max) == 0) return 0;
	if(inside_plane(Planes[1],min,max) == 0) return 0;
	if(inside_plane(Planes[2],min,max) == 0) return 0;
	if(inside_plane(Planes[3],min,max) == 0) return 0;
	if(inside_plane(Planes[4],min,max) == 0) return 0;
	if(inside_plane(Planes[5],min,max) == 0) return 0;
	return 1;
}

int VBoundFrustum::InsidePlanes(const VVector3 *Points,int num_points) const {
	if(inside_plane(Planes[0],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[1],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[2],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[3],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[4],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[5],Points,num_points) == 0) return 0;
	return 1;
}

/*
 */
int VBoundFrustum::inside_planes_fast(const VVector3& point) const {
	#ifdef V_SSE
		__m128 res_0 = _mm_mul_ps(TPlanes[0].vec,_mm_set1_ps(point.x));
		__m128 res_1 = _mm_mul_ps(TPlanes[1].vec,_mm_set1_ps(point.y));
		__m128 res_2 = _mm_mul_ps(TPlanes[2].vec,_mm_set1_ps(point.z));
		__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[3].vec));
		if(_mm_movemask_ps(res_3) & 0x0f) return 0;
	#elif USE_ALTIVEC
		vec_float4 res_0 = vec_madd(TPlanes[0].vec,vec_splats(point.x),TPlanes[3].vec);
		vec_float4 res_1 = vec_madd(TPlanes[1].vec,vec_splats(point.y),res_0);
		vec_float4 res_2 = vec_madd(TPlanes[2].vec,vec_splats(point.z),res_1);
		if(VEC_SWIZZLE((vec_uint4)res_2,B0,B0,B0,B0)[0] & 0x80808080) return 0;
	#elif USE_NEON
		float32x4_t res_0 = vmlaq_n_f32(TPlanes[3].vec,TPlanes[0].vec,point.x);
		float32x4_t res_1 = vmlaq_n_f32(res_0,TPlanes[1].vec,point.y);
		float32x4_t res_2 = vmlaq_n_f32(res_1,TPlanes[2].vec,point.z);
		if(vmaskq_u32(vcltq_f32(res_2,vdupq_n_f32(0.0f)))) return 0;
	#else
		if(dot(Planes[0],point) < 0.0f) return 0;
		if(dot(Planes[1],point) < 0.0f) return 0;
		if(dot(Planes[2],point) < 0.0f) return 0;
		if(dot(Planes[3],point) < 0.0f) return 0;
	#endif
	return 1;
}

bool VBoundFrustum::InsidePlanesFast(const VVector3& point,float radius) const {
	#ifdef V_SSE
		__m128 res_0 = _mm_mul_ps(TPlanes[0].vec,_mm_set1_ps(point.x));
		__m128 res_1 = _mm_mul_ps(TPlanes[1].vec,_mm_set1_ps(point.y));
		__m128 res_2 = _mm_mul_ps(TPlanes[2].vec,_mm_set1_ps(point.z));
		__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,TPlanes[3].vec));
		if(_mm_movemask_ps(_mm_add_ps(res_3,_mm_set1_ps(radius))) & 0x0f) return false;
	#elif USE_ALTIVEC
		vec_float4 res_0 = vec_madd(TPlanes[0].vec,vec_splats(point.x),TPlanes[3].vec);
		vec_float4 res_1 = vec_madd(TPlanes[1].vec,vec_splats(point.y),res_0);
		vec_float4 res_2 = vec_madd(TPlanes[2].vec,vec_splats(point.z),res_1);
		vec_uint4 res_3 = (vec_uint4)vec_add(res_2,vec_splats(radius));
		if(VEC_SWIZZLE(res_3,B0,B0,B0,B0)[0] & 0x80808080) return 0;
	#elif USE_NEON
		float32x4_t res_0 = vmlaq_n_f32(TPlanes[3].vec,TPlanes[0].vec,point.x);
		float32x4_t res_1 = vmlaq_n_f32(res_0,TPlanes[1].vec,point.y);
		float32x4_t res_2 = vmlaq_n_f32(res_1,TPlanes[2].vec,point.z);
		if(vmaskq_u32(vcltq_f32(res_2,vdupq_n_f32(-radius)))) return 0;
	#else
	for (int i = 0; i < 4; ++i)
	{
		if (Planes[i].Dot(point) < -radius)
		{
			return false;
		}
	}
	return true;
	#endif
	return true;
}


bool VBoundFrustum::InsidePlanesFast(const VVector3& min,const VVector3& max, int NumPlanes) const 
{
	for(int i = 0; i <NumPlanes;++i)
	{
		if (!inside_plane(Planes[i],min,max))
		{
			return false;
		}
	}
	return true;
}

int VBoundFrustum::inside_planes_fast(const VVector3 *Points,int num_points) const {
	if(inside_plane(Planes[0],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[1],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[2],Points,num_points) == 0) return 0;
	if(inside_plane(Planes[3],Points,num_points) == 0) return 0;
	return 1;
}

/*
 */
inline bool VBoundFrustum::InsidePortal(const Portal& portal,const VVector3& point) const 
{
	if (portal.Plane.Dot(point) < 0.0f)
	{
		return false;
	}
	
	#ifdef V_SSE
		__m128 res_0 = _mm_mul_ps(portal.TPlanes[0].vec,_mm_set1_ps(point.x));
		__m128 res_1 = _mm_mul_ps(portal.TPlanes[1].vec,_mm_set1_ps(point.y));
		__m128 res_2 = _mm_mul_ps(portal.TPlanes[2].vec,_mm_set1_ps(point.z));
		__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,portal.TPlanes[3].vec));
		if(_mm_movemask_ps(res_3)) return false;
	#elif USE_ALTIVEC
		vec_float4 res_0 = vec_madd(portal.TPlanes[0].vec,vec_splats(point.x),portal.TPlanes[3].vec);
		vec_float4 res_1 = vec_madd(portal.TPlanes[1].vec,vec_splats(point.y),res_0);
		vec_float4 res_2 = vec_madd(portal.TPlanes[2].vec,vec_splats(point.z),res_1);
		if(VEC_SWIZZLE((vec_uint4)res_2,B0,B0,B0,B0)[0] & 0x80808080) return 0;
	#elif USE_NEON
		float32x4_t res_0 = vmlaq_n_f32(portal.TPlanes[3].vec,portal.TPlanes[0].vec,point.x);
		float32x4_t res_1 = vmlaq_n_f32(res_0,portal.TPlanes[1].vec,point.y);
		float32x4_t res_2 = vmlaq_n_f32(res_1,portal.TPlanes[2].vec,point.z);
		if(vmaskq_u32(vcltq_f32(res_2,vdupq_n_f32(0.0f)))) return 0;
	#else
		if(dot(portal.Planes[0],point) < 0.0f) return 0;
		if(dot(portal.Planes[1],point) < 0.0f) return 0;
		if(dot(portal.Planes[2],point) < 0.0f) return 0;
		if(dot(portal.Planes[3],point) < 0.0f) return 0;
	#endif
	return true;
}

inline bool VBoundFrustum::InsidePortal(const Portal& portal,const VVector3& point,float radius) const 
{
	if (portal.Plane.Dot(point) < -radius)
	{
		return false;
	}

	#ifdef V_SSE
		__m128 res_0 = _mm_mul_ps(portal.TPlanes[0].vec,_mm_set1_ps(point.x));
		__m128 res_1 = _mm_mul_ps(portal.TPlanes[1].vec,_mm_set1_ps(point.y));
		__m128 res_2 = _mm_mul_ps(portal.TPlanes[2].vec,_mm_set1_ps(point.z));
		__m128 res_3 = _mm_add_ps(_mm_add_ps(res_0,res_1),_mm_add_ps(res_2,portal.TPlanes[3].vec));
		if(_mm_movemask_ps(_mm_add_ps(res_3,_mm_set1_ps(radius)))) return false;
	#elif USE_ALTIVEC
		vec_float4 res_0 = vec_madd(portal.TPlanes[0].vec,vec_splats(point.x),portal.TPlanes[3].vec);
		vec_float4 res_1 = vec_madd(portal.TPlanes[1].vec,vec_splats(point.y),res_0);
		vec_float4 res_2 = vec_madd(portal.TPlanes[2].vec,vec_splats(point.z),res_1);
		vec_uint4 res_3 = (vec_uint4)vec_add(res_2,vec_splats(radius));
		if(VEC_SWIZZLE(res_3,B0,B0,B0,B0)[0] & 0x80808080) return 0;
	#elif USE_NEON
		float32x4_t res_0 = vmlaq_n_f32(portal.TPlanes[3].vec,portal.TPlanes[0].vec,point.x);
		float32x4_t res_1 = vmlaq_n_f32(res_0,portal.TPlanes[1].vec,point.y);
		float32x4_t res_2 = vmlaq_n_f32(res_1,portal.TPlanes[2].vec,point.z);
		if(vmaskq_u32(vcltq_f32(res_2,vdupq_n_f32(-radius)))) return 0;
	#else
		if(dot(portal.Planes[0],point) < -radius) return 0;
		if(dot(portal.Planes[1],point) < -radius) return 0;
		if(dot(portal.Planes[2],point) < -radius) return 0;
		if(dot(portal.Planes[3],point) < -radius) return 0;
	#endif
	return true;
}

inline bool VBoundFrustum::InsidePortal(const Portal& portal,const VVector3& min,const VVector3& max) const 
{
	if(!inside_plane(portal.Plane,min,max)) return false;
	if(!inside_plane(portal.Planes[0],min,max)) return false;
	if(!inside_plane(portal.Planes[1],min,max)) return false;
	if(!inside_plane(portal.Planes[2],min,max)) return false;
	if(!inside_plane(portal.Planes[3],min,max)) return false;
	return true;
}

inline bool VBoundFrustum::InsidePortal(const Portal& portal,const VVector3 *Points,int num_points) const 
{
	if(!inside_plane(portal.Plane,Points,num_points)) return false;
	if(!inside_plane(portal.Planes[0],Points,num_points)) return false;
	if(!inside_plane(portal.Planes[1],Points,num_points)) return false;
	if(!inside_plane(portal.Planes[2],Points,num_points)) return false;
	if(!inside_plane(portal.Planes[3],Points,num_points)) return false;
	return true;
}


int VBoundFrustum::InsidePortals(const VVector3& point,float radius) const {
	for(int i = 0; i < Portals.Size(); i++) {
		if(InsidePortal(Portals[i],point,radius) == 0) return 0;
	}
	return 1;
}

int VBoundFrustum::InsidePortals(const VVector3& min,const VVector3& max) const {
	for(int i = 0; i < Portals.Size(); i++) {
		if(InsidePortal(Portals[i],min,max) == 0) return 0;
	}
	return 1;
}

int VBoundFrustum::InsidePortals(const VVector3 *Points,int num_points) const {
	for(int i = 0; i < Portals.Size(); i++) {
		if(InsidePortal(Portals[i],Points,num_points) == 0) return 0;
	}
	return 1;
}

/*
 */
int VBoundFrustum::insideAll(const VBoundSphere &bs) const {
	//if(bs.isValid()) return insideAllValid(bs);
	return 0;
}

int VBoundFrustum::insideAll(const VBoundBox &bb) const {
//	if(bb.isValid()) return insideAllValid(bb);
	return 0;
}

int VBoundFrustum::insideAll(const VBoundFrustum& bf) const {
	if(bf.isValid()) return insideAllValid(bf);
	return 0;
}

/*
 */
int VBoundFrustum::insidePlanes(const VBoundSphere &bs) const {
//	if(bs.isValid()) return insidePlanesValid(bs);
	return 0;
}

int VBoundFrustum::insidePlanes(const VBoundBox &bb) const {
//	if(bb.isValid()) return insidePlanesValid(bb);
	return 0;
}

int VBoundFrustum::insidePlanes(const VBoundFrustum& bf) const {
	if(bf.isValid()) return insidePlanesValid(bf);
	return 0;
}

/*
 */
int VBoundFrustum::insidePortals(const VBoundSphere &bs) const {
//	if(bs.isValid()) return insidePortalsValid(bs);
	return 0;
}

int VBoundFrustum::insidePortals(const VBoundBox &bb) const {
//	if(bb.isValid()) return insidePortalsValid(bb);
	return 0;
}

int VBoundFrustum::insidePortals(const VBoundFrustum& bf) const {
//	if(bf.isValid()) return insidePortalsValid(bf);
	return 0;
}

/*
 */
bool VBoundFrustum::InsideShadow(const VVector3& LightDirection, const VBoundSphere& BoundSphere) const
{
	
	// object is Inside the bound frustum
	if(InsidePlanes(BoundSphere.Center, BoundSphere.Radius))
	{
		return true;
	}

	// extruded shadow volume is Inside the bound frustum
	float dot;
	VVector3 ExtrudedCenter;
	for(int i = 0; i < 6; i++) 
	{
		const VVector4& Plane = Planes[i];
		dot = Plane.Dot3(LightDirection);

		// 如果光线与平面近似平行, 忽略该平面
		if (VAbs(dot) < 0.001f)
		{
			continue;
		}
		
		// 计算从包围球中心沿光线方向到平面交点的距离
		dot = -Plane.Dot(BoundSphere.Center)/dot;
		if (dot > BoundSphere.Radius)
		{
			continue;
		}
		ExtrudedCenter = BoundSphere.Center + LightDirection * dot;

		if (InsidePlanes(ExtrudedCenter,  BoundSphere.Radius))
		{
			return true;
		}
		

		/*float k = dot3(Planes[i],direction);
		if(Math::abs(k) < EPSILON) continue;

		k = -dot(Planes[i],object.getCenter()) / k;
		if(k > object.getRadius()) continue;
		if(InsidePlanes(object.getCenter() + direction * k,object.getRadius())) return 1;*/
	}
	return false;
}

int VBoundFrustum::insideShadow(const VBoundSphere &light,const VVector3& offset,const VBoundSphere &object) const 
{
	//
	//if(light.isValid() && object.isValid()) {
	//	
	//	// object is outside the light bounds
	//	if(light.Inside(object) == 0) return 0;
	//	
	//	// object is Inside the bound frustum
	//	if(InsidePlanes(object.getCenter(),object.getRadius())) return 1;
	//	
	//	// direction from light center to object center
	//	VVector3 direction = object.getCenter() - offset;
	//	float distance = length(direction);
	//	if(distance < object.getRadius() + EPSILON) return 1;
	//	direction /= distance;
	//	
	//	// basis
	//	VVector3 x,y;
	//	orthoBasis(direction,x,y);
	//	
	//	// near Points
	//	VVector3 x0 = x * object.getRadius();
	//	VVector3 y0 = y * object.getRadius();
	//	VVector3 z0 = offset + direction * (distance - object.getRadius());
	//	
	//	// far Points
	//	float radius = light.getRadius() + dot(direction,light.getCenter() - offset);
	//	float k = object.getRadius() * radius / (distance - object.getRadius());
	//	VVector3 x1 = x * k;
	//	VVector3 y1 = y * k;
	//	VVector3 z1 = offset + direction * radius;
	//	
	//	// check visibility
	//	VVector3 Points[8] = {
	//		x0 + y0 + z0,x0 - y0 + z0,-x0 - y0 + z0,-x0 + y0 + z0,
	//		x1 + y1 + z1,x1 - y1 + z1,-x1 - y1 + z1,-x1 + y1 + z1,
	//	};
	//	
	//	return InsidePlanes(Points,8);
	//}
	return 0;
}



#endif 