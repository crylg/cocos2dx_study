#include "VStdAfx.h"
#include "VMath.h"
#include "VPackedFloat.h"
#include "VTransform.h"
const float VHalf::SCALE_FACTOR = 1.0f/65536.0f;
const float VHalf::INV_SCALE_FACTOR = 65536.0f;




// SIMD opitimization
#if V_SSE
//_mm_load_ps
__forceinline __m128 sse_load_is(int x,int y,int z,int w)
{
	union { __m128 v; int f[4]; } Tmp;
	Tmp.f[0] = x;
	Tmp.f[1] = y;
	Tmp.f[2] = z;
	Tmp.f[3] = w;
	return Tmp.v;
}
__m128 VSSE_CR1_11_1 = sse_load_is(1,-1,1,-1);
__m128 VSSE_CR11_1_1 = sse_load_is(1,1,-1,-1);
__m128 VSSE_CR_111_1 = sse_load_is(-1,1,1,-1);
#elif V_NEON

#endif 

const VVector2 VVector2::ZERO(0.0f);
const VVector2 VVector2::ONE(1.0f);

// constants
const VVector3 VVec3_0(0.0f,0.0f,0.0f);
const VVector3 VVec3_1(1.0f,1.0f,1.0f);
const VVector3 VVec3_X(1.0f,0.0f,0.0f);
const VVector3 VVec3_Y(0.0f,1.0f,0.0f);
const VVector3 VVec3_Z(0.0f,0.0f,1.0f);
const VVector3 VVec3_EPS(VEPS, VEPS, VEPS);
const VVector3 VVec3_INF(VINFINITY, VINFINITY, VINFINITY);


const VVector3 VVector3::XAXIS(1.0f,0.0f,0.0f);
const VVector3 VVector3::YAXIS(0.0f,1.0f,0.0f);
const VVector3 VVector3::ZAXIS(0.0f,0.0f,1.0f);
const VVector3 VVector3::NEG_XAXIS(-1.0f,0.0f,0.0f);
const VVector3 VVector3::NEG_YAXIS(0.0f,-1.0f,0.0f);
const VVector3 VVector3::NEG_ZAXIS(0.0f,0.0f,-1.0f);
const VVector3 VVector3::ZERO(0.0f,0.0f,0.0f);
const VVector3 VVector3::ONE(1.0f,1.0f,1.0f);


const VVector4 VVector4::ZERO(0.0f,0.0f,0.0f,0.0f);
const VVector4 VVector4::ONE(1.0f,1.0f,1.0f,1.0f);
const VQuat VQuat::IDENTITY(0.0f,0.0f,0.0f,1.0f);


//! 计算与this正交的2条基底坐标轴(不定解), 返回值为单位矢量
void VVector3::GetBaseAxis(VVector3& vAxis1, VVector3& vAxis2)
{
	float NX = VAbs(x);
	float NY = VAbs(y);
	float NZ = VAbs(z);

	// Find best basis vectors.
	if( NZ>NX && NZ>NY )	
		vAxis1 = VVec3_X;
	else
		vAxis1 = VVec3_Z;

	// 获得垂直的矢量(也可以考虑用 a2 = a1 X this  a1 = a2 X this 来计算, 但是效率稍微低点)
	vAxis1 = vAxis1 - *this * (vAxis1 | *this);
	vAxis1.Normalize();
	vAxis2 = vAxis1 ^ *this;
}


void VMatrix::Decompose(VVector3& Scale, VQuat& Rot, VVector3& Transltate) const
{
	VMatrix RotScaleMat(*this);
																			RotScaleMat.m14 = 0.0f;
																			RotScaleMat.m24 = 0.0f;
																			RotScaleMat.m34 = 0.0f;
	RotScaleMat.m41 = 0.0f; RotScaleMat.m42 = 0.0f; RotScaleMat.m43 = 0.0f; RotScaleMat.m44 = 1.0f;

	VMatrix RotMat;
	// make orthonormalize 
	{
		VVector3 X(m11,m12,m13);
		VVector3 Y(m21,m22,m23);
		VVector3 Z = X.Cross(Y);
		Y = Z.Cross(X);
		X.Normalize();
		Y.Normalize();
		Z.Normalize();

		RotMat.m11 = X.x; RotMat.m12 = X.y; RotMat.m13 = X.z; RotMat.m14 = 0.0f;
		RotMat.m21 = Y.x; RotMat.m22 = Y.y; RotMat.m23 = Y.z; RotMat.m24 = 0.0f;
		RotMat.m31 = Z.x; RotMat.m32 = Z.y; RotMat.m33 = Z.z; RotMat.m34 = 0.0f;
		RotMat.m41 = 0.0f; RotMat.m42 = 0.0f; RotMat.m43 = 0.0f; RotMat.m44 = 1.0f;
	}
	VMatrix InvRot = RotMat.Transpose();
	VMatrix ScaleMat = RotScaleMat * InvRot;

	Transltate.x = m41; Transltate.y = m42; Transltate.z = m43;
	Rot.FromRotateMatrix(RotMat);
	Scale.Set(ScaleMat.m11,ScaleMat.m22, ScaleMat.m33);
}

void VMatrix::DecomposeFast(VVector3& Scale, VQuat& Rot, VVector3& Transltate) const
{
	VMatrix Mat(*this);

	// 抽取去scale;
	Scale.Set(1.0f,1.0f,1.0f);

	const float LenX = (Mat.m[0][0] * Mat.m[0][0]) + (Mat.m[0][1] * Mat.m[0][1]) + (Mat.m[0][2] * Mat.m[0][2]);
	const float LenY = (Mat.m[1][0] * Mat.m[1][0]) + (Mat.m[1][1] * Mat.m[1][1]) + (Mat.m[1][2] * Mat.m[1][2]);
	const float LenZ = (Mat.m[2][0] * Mat.m[2][0]) + (Mat.m[2][1] * Mat.m[2][1]) + (Mat.m[2][2] * Mat.m[2][2]);

	if( LenX > VEPS )
	{
		Scale.x = VSqrt(LenX);
		float Inv = 1.0f / Scale.x;
		Mat.m11 *= Inv;Mat.m12 *= Inv;Mat.m13 *= Inv;
	}


	if( LenY > VEPS )
	{
		Scale.y = VSqrt(LenY);
		float Inv = 1.0f / Scale.y;
		Mat.m21 *= Inv;Mat.m22 *= Inv;Mat.m23 *= Inv;
	}


	if( LenZ > VEPS )
	{
		Scale.z = VSqrt(LenZ);
		float Inv = 1.0f / Scale.z;
		Mat.m31 *= Inv;Mat.m32 *= Inv;Mat.m33 *= Inv;
	}


	Rot.FromRotateMatrix(Mat);
	Rot.Normalize();
	Transltate.x = m41; Transltate.y = m42; Transltate.z = m43;

}


/*
   关于VQS transform



*/
#if 0
void TestVQSTransform()
{
	VTransform ParentToWorldTransform;
	ParentToWorldTransform.Rotation = VQuat(VVec3_Z, VDEG_RAD(45.0f));
	ParentToWorldTransform.Translation = VVec3_0;
	ParentToWorldTransform.Scale = VVector3(1,2,8);

	VMatrix Mat_p2w = ParentToWorldTransform.ToMatrix();
	VMatrix Mat_w2p = Mat_p2w.Inverse();



	VTransform BToWorld;
	BToWorld.Rotation = VQuat(VVec3_Z, VDEG_RAD(90.0f));
	BToWorld.Translation = VVector3(1,1,0);
	BToWorld.Scale = VVector3(1,2,6);

	VMatrix Mat_b2w = BToWorld.ToMatrix();
	VMatrix Mat_w2b = Mat_b2w.Inverse();
	VMatrix Mat_b2p = Mat_b2w * Mat_w2p;

	VTransform BTP(Mat_b2p);		// bad !!!!  concatenated with inverse matrix.

	VTransform BToParent = BToWorld.GetRelativeTransform(ParentToWorldTransform);

	VTransform BToWorld2 = BToParent * ParentToWorldTransform;			// works . 



	VVector3 P(2,2,0);

	VVector3 Result = BToWorld.TransformPosition(P);
	Result = Result * Mat_w2p;
	Result = ParentToWorldTransform.TransformPosition(Result);
	Result = Result * Mat_w2b;

	VVector3 Result2;
	Result2 = BToParent.TransformPosition(P);
	Result = ParentToWorldTransform.TransformPosition(Result2);
	Result = Result * Mat_w2b;
	Result2.x = Result.x;
}

#endif 