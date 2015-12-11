#ifndef __VTRANSFORM_H__
#define __VTRANSFORM_H__
#include "VMath.h"
// ideal taken from gamebryo.


class VTransform
{
public:
	VQuat		Rotation;
	VVector3	Translation;
	VVector3	Scale;			// Note: 这里如果是用 uniform scale 的话, 将会更加高效
	inline VTransform(void)
	{
	}
	inline explicit VTransform(const VVector3& Translate) 
		:Rotation(VQuat::IDENTITY),Translation(Translate), Scale(1.0f)
	{
	}
	inline explicit VTransform(const VQuat& Rot) 
		:Rotation(Rot),Translation(VVec3_0), Scale(1.0f)
	{
	}
	inline VTransform(const VQuat& Rot, const VVector3& Translate, const VVector3& Scl = VVec3_1) 
		:Rotation(Rot),Translation(Translate), Scale(Scl)
	{
	}

	inline VTransform(const VTransform& InTransform) 
		: Rotation(InTransform.Rotation), Translation(InTransform.Translation), Scale(InTransform.Scale)
	{
	}

	inline explicit VTransform(const VMatrix& Matrix)
	{
		SetFromMatrix(Matrix);
	}

	static inline BOOL IsValidScale(const VVector3& Scale)
	{
		return Scale.x >0.001f && Scale.y > 0.001f && Scale.z > 0.001f;
	}

	inline void ToMatrix(VMatrix& OutMatrix) const
	{
		VASSERT(Rotation.IsNormalized());
		OutMatrix.m[3][0] = Translation.x;
		OutMatrix.m[3][1] = Translation.y;
		OutMatrix.m[3][2] = Translation.z;

		const float x2 = Rotation.x + Rotation.x;	
		const float y2 = Rotation.y + Rotation.y;  
		const float z2 = Rotation.z + Rotation.z;
		{
			const float xx2 = Rotation.x * x2;
			const float yy2 = Rotation.y * y2;			
			const float zz2 = Rotation.z * z2;

			OutMatrix.m[0][0] = (1.0f - (yy2 + zz2)) * Scale.x;	
			OutMatrix.m[1][1] = (1.0f - (xx2 + zz2)) * Scale.y;
			OutMatrix.m[2][2] = (1.0f - (xx2 + yy2)) * Scale.z;
		}
		{
			const float yz2 = Rotation.y * z2;   
			const float wx2 = Rotation.w * x2;	

			OutMatrix.m[2][1] = (yz2 - wx2) * Scale.z;
			OutMatrix.m[1][2] = (yz2 + wx2) * Scale.y;
		}
		{
			const float xy2 = Rotation.x * y2;
			const float wz2 = Rotation.w * z2;

			OutMatrix.m[1][0] = (xy2 - wz2) * Scale.y;
			OutMatrix.m[0][1] = (xy2 + wz2) * Scale.x;
		}
		{
			const float xz2 = Rotation.x * z2;
			const float wy2 = Rotation.w * y2;   

			OutMatrix.m[2][0] = (xz2 + wy2) * Scale.z;
			OutMatrix.m[0][2] = (xz2 - wy2) * Scale.x;
		}

		OutMatrix.m[0][3] = 0.0f;
		OutMatrix.m[1][3] = 0.0f;
		OutMatrix.m[2][3] = 0.0f;
		OutMatrix.m[3][3] = 1.0f;
	}

	inline VMatrix ToMatrix() const
	{
		VMatrix OutMatrix;
		ToMatrix(OutMatrix);
		return OutMatrix;
	}

	/**
	* Convert this Transform to matrix with scaling and compute the inverse of that.
	*/
	inline VMatrix ToInverseMatrixWithScale() const
	{
		// todo: optimize
		return ToMatrix().Inverse();
	}

	/**
	* Convert this Transform to inverse.
	*/
	inline VTransform InverseSafe() const
	{
		// todo: optimize
		return VTransform(ToMatrix().Inverse());
	}

	/**
	* Convert this Transform to a transformation matrix, ignoring its scaling
	*/
	inline VMatrix ToMatrixNoScale() const
	{
		VMatrix OutMatrix;

		OutMatrix.m[3][0] = Translation.x;
		OutMatrix.m[3][1] = Translation.y;
		OutMatrix.m[3][2] = Translation.z;

		const float x2 = Rotation.x + Rotation.x;	
		const float y2 = Rotation.y + Rotation.y;  
		const float z2 = Rotation.z + Rotation.z;
		{
			const float xx2 = Rotation.x * x2;
			const float yy2 = Rotation.y * y2;			
			const float zz2 = Rotation.z * z2;

			OutMatrix.m[0][0] = (1.0f - (yy2 + zz2));	
			OutMatrix.m[1][1] = (1.0f - (xx2 + zz2));
			OutMatrix.m[2][2] = (1.0f - (xx2 + yy2));
		}
		{
			const float yz2 = Rotation.y * z2;   
			const float wx2 = Rotation.w * x2;	

			OutMatrix.m[2][1] = (yz2 - wx2);
			OutMatrix.m[1][2] = (yz2 + wx2);
		}
		{
			const float xy2 = Rotation.x * y2;
			const float wz2 = Rotation.w * z2;

			OutMatrix.m[1][0] = (xy2 - wz2);
			OutMatrix.m[0][1] = (xy2 + wz2);
		}
		{
			const float xz2 = Rotation.x * z2;
			const float wy2 = Rotation.w * y2;   

			OutMatrix.m[2][0] = (xz2 + wy2);
			OutMatrix.m[0][2] = (xz2 - wy2);
		}

		OutMatrix.m[0][3] = 0.0f;
		OutMatrix.m[1][3] = 0.0f;
		OutMatrix.m[2][3] = 0.0f;
		OutMatrix.m[3][3] = 1.0f;

		return OutMatrix;
	}

	/** Set this transform to the weighted blend of the supplied two transforms. */
	inline void Blend(const VTransform& Atom1, const VTransform& Atom2, float Alpha)
	{
//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) && WITH_EDITORONLY_DATA
//		// Check that all bone atoms coming from animation are normalized
//		check( Atom1.IsRotationNormalized() );
//		check( Atom2.IsRotationNormalized() );
//#endif
//		if( Alpha <= ZERO_ANIMWEIGHT_THRESH )
//		{
//			// if blend is all the way for child1, then just copy its bone atoms
//			(*this) = Atom1;
//		}
//		else if( Alpha >= 1.f - ZERO_ANIMWEIGHT_THRESH )
//		{
//			// if blend is all the way for child2, then just copy its bone atoms
//			(*this) = Atom2;
//		}
//		else
//		{
//			// Simple linear interpolation for translation and scale.
//			Translation = FMath::Lerp(Atom1.Translation, Atom2.Translation, Alpha);
//			Scale		= FMath::Lerp(Atom1.Scale, Atom2.Scale, Alpha);
//			Rotation	= VQuat::FastLerp(Atom1.Rotation, Atom2.Rotation, Alpha);
//
//			// ..and renormalize
//			Rotation.Normalize();
//		}
	}

	/** Set this Transform to the weighted blend of it and the supplied Transform. */
	inline void BlendWith(const VTransform& OtherAtom, float Alpha)
	{
//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST) && WITH_EDITORONLY_DATA
//		// Check that all bone atoms coming from animation are normalized
//		check( IsRotationNormalized() );
//		check( OtherAtom.IsRotationNormalized() );
//#endif
//		if( Alpha > ZERO_ANIMWEIGHT_THRESH )
//		{
//			if( Alpha >= 1.f - ZERO_ANIMWEIGHT_THRESH )
//			{
//				// if blend is all the way for child2, then just copy its bone atoms
//				(*this) = OtherAtom;
//			}
//			else 
//			{
//				// Simple linear interpolation for translation and scale.
//				Translation = FMath::Lerp(Translation, OtherAtom.Translation, Alpha);
//				Scale		= FMath::Lerp(Scale, OtherAtom.Scale, Alpha);
//				Rotation	= VQuat::FastLerp(Rotation, OtherAtom.Rotation, Alpha);
//
//				// ..and renormalize
//				Rotation.Normalize();
//			}
//		}
	}


	/**
	 * Quaternion addition is wrong here. This is just a special case for linear interpolation.
	 * Use only within blends!!
	 * Rotation part is NOT normalized!!
	 */
	inline VTransform operator+(const VTransform& Atom) const
	{
		return VTransform(Rotation + Atom.Rotation, Translation + Atom.Translation, Scale + Atom.Scale);
	}

	inline VTransform& operator+=(const VTransform& Atom)
	{
		Translation += Atom.Translation;

		Rotation.x += Atom.Rotation.x;
		Rotation.y += Atom.Rotation.y;
		Rotation.z += Atom.Rotation.z;
		Rotation.w += Atom.Rotation.w;

		Scale += Atom.Scale;

		return *this;
	}

	/*inline VTransform operator*(ScalarRegister Mult) const
	{
		return VTransform(Rotation * Mult, Translation * Mult, Scale * Mult);
	}

	inline VTransform& operator*=(ScalarRegister Mult)
	{
		Translation *= Mult;
		Rotation.x	*= Mult;
		Rotation.y	*= Mult;
		Rotation.z	*= Mult;
		Rotation.w	*= Mult;
		Scale		*= Mult;
		DiagnosticCheckNaN_All();

		return *this;
	}*/

	inline VTransform		operator*(const VTransform& Other) const;
	inline void			operator*=(const VTransform& Other);
	inline VTransform		operator*(const VQuat& Other) const;
	inline void			operator*=(const VQuat& Other);

	inline void ScaleTranslation(const VVector3& Scale);
	inline void ScaleTranslation(const float& Scale);
	inline void RemoveScaling();
	inline float GetMaximumAxisScale() const;
	inline float GetMinimumAxisScale() const;
	// Inverse does not work well with VQS format(in particular non-uniform), so removing it, but made two below functions to be used instead. 

	/*******************************************************************************************
	 * The below 2 functions are the ones to get delta transform and return VTransform format that can be concatenated
	 * Inverse itself can't concatenate with VQS format(since VQS always transform from S->Q->T, where inverse happens from T(-1)->Q(-1)->S(-1))
	 * So these 2 provides ways to fix this
	 * GetRelativeTransform returns this*Other(-1) and parameter is Other(not Other(-1))
	 * GetRelativeTransformReverse returns this(-1)*Other, and parameter is Other. 
	 *******************************************************************************************/

	/// VQS transform 在non-uniform scaling 中,逆变换工作不正常. 但是这不代表不能使用.
	/// 注意. RelativeTransform 和矩阵不要混用. RelativeTransform 
	inline VTransform GetRelativeTransform(const VTransform& Other) const;

	VCoreApi VTransform GetRelativeTransformReverse(const VTransform& Other) const;
	/**
	 * Set current transform and the relative to ParentTransform.
	 * Equates to This = This->GetRelativeTransform(Parent), but saves the intermediate VTransform storage and copy.
	 */
	VCoreApi void		SetToRelativeTransform(const VTransform& ParentTransform);

	inline VVector4		TransformFVector4(const VVector4& V) const;
	inline VVector4		TransformFVector4NoScale(const VVector4& V) const;
	inline VVector3		TransformPosition(const VVector3& V) const;


	/** Inverts the matrix and then transforms V - correctly handles scaling in this matrix. */
	inline VVector3		InverseTransformPosition(const VVector3 &V) const;

	inline VVector3		InverseTransformPositionNoScale(const VVector3 &V) const;

	inline VVector3		TransformVector(const VVector3& V) const;

	inline VVector3		TransformVectorNoScale(const VVector3& V) const;

	/** 
	 *	Transform a direction vector by the inverse of this matrix - will not take into account translation part.
	 *	If you want to transform a surface normal (or plane) and correctly account for non-uniform scaling you should use TransformByUsingAdjointT with adjoint of matrix inverse.
	 */
	inline VVector3 InverseTransformVector(const VVector3 &V) const;
	inline VVector3 InverseTransformVectorNoScale(const VVector3 &V) const;
	inline VTransform	GetScaled(float Scale) const;
	inline VTransform	GetScaled(VVector3 Scale) const;
	inline VVector3		GetSafeScaleReciprocal(const VVector3 & InScale) const;
	// temp function for easy conversion
	inline VVector3 GetLocation() const
	{
		return Translation;
	}

	/** Calculate the  */
	inline float GetDeterminant() const
	{
		return Scale.x * Scale.y * Scale.z;
	}

	/** Set the translation of this transformation */
	inline void SetLocation(const VVector3& Origin)
	{
		Translation = Origin;
	}


	inline static void Multiply(VTransform * OutTransform, const VTransform * A, const VTransform * B);

	/**
	 * Sets the components
	 * @param InRotation The new value for the Rotation component
	 * @param InTranslation The new value for the Translation component
	 * @param InScale3D The new value for the Scale component
	 */
	inline void SetComponents(const VQuat& InRotation, const VVector3& InTranslation, const VVector3& InScale3D) 
	{
		Rotation = InRotation;
		Translation = InTranslation;
		Scale = InScale3D;
	}

	inline void SetIdentity()
	{
		Rotation.Identity();
		Translation.Set(0.0f,0.0f,0.0f);
		Scale.Set(1.0f,1.0f,1.0f);
	}

	inline void MultiplyScale3D(const VVector3& Scale3DMultiplier)
	{
		Scale *= Scale3DMultiplier;
	}

	inline void SetTranslation(const VVector3& NewTranslation)
	{
		Translation = NewTranslation;
	}

	inline void SetScale3D(const VVector3& NewScale3D)
	{
		Scale = NewScale3D;
	}

	/**
	 * Sets both the translation and Scale components at the same time
	 * @param NewTranslation The new value for the translation component
	 * @param NewScale3D The new value for the Scale component
	 */
	inline void SetTranslationAndScale3D(const VVector3& NewTranslation, const VVector3& NewScale3D)
	{
		Translation = NewTranslation;
		Scale = NewScale3D;
	}

	/**
	 * Accumulates another transform with this one, with an optional blending weight
	 *
	 * Rotation is accumulated additively, in the shortest direction (Rotation = Rotation +/- DeltaAtom.Rotation * Weight)
	 * Translation is accumulated additively (Translation += DeltaAtom.Translation * Weight)
	 * Scale is accumulated additively (Scale += DeltaAtom.Scale * Weight)
	 *
	 * @param DeltaAtom The other transform to accumulate into this one
	 * @param Weight The weight to multiply DeltaAtom by before it is accumulated.
	 */
	inline void AccumulateWithShortestRotation(const VTransform& DeltaAtom, float Weight = 1.0f)
	{
		//VTransform Atom(DeltaAtom * Weight);

		//// To ensure the 'shortest route', we make sure the dot product between the accumulator and the incoming child atom is positive.
		//if( (Atom.Rotation | Rotation) < 0.f )
		//{
		//	Rotation.x -= Atom.Rotation.x;
		//	Rotation.y -= Atom.Rotation.y;
		//	Rotation.z -= Atom.Rotation.z;
		//	Rotation.w -= Atom.Rotation.w;
		//}
		//else
		//{
		//	Rotation.x += Atom.Rotation.x;
		//	Rotation.y += Atom.Rotation.y;
		//	Rotation.z += Atom.Rotation.z;
		//	Rotation.w += Atom.Rotation.w;
		//}

		//Translation += Atom.Translation;
		//Scale += Atom.Scale;
	}

	/**
	 * Accumulates another transform with this one
	 *
	 * Rotation is accumulated multiplicatively (Rotation = SourceAtom.Rotation * Rotation)
	 * Translation is accumulated additively (Translation += SourceAtom.Translation)
	 * Scale is accumulated multiplicatively (Scale *= SourceAtom.Scale)
	 *
	 * @param SourceAtom The other transform to accumulate into this one
	 */
	inline void Accumulate(const VTransform& SourceAtom)
	{
		//// Add ref pose relative animation to base animation, only if rotation is significant.
		//if( FMath::Square(SourceAtom.Rotation.w) < 1.f - DELTA * DELTA )
		//{
		//	Rotation = SourceAtom.Rotation * Rotation;
		//}

		//Translation += SourceAtom.Translation;
		//Scale *= SourceAtom.Scale;

	}

   /** Accumulates another transform with this one, with a blending weight
	*
	* Let SourceAtom = Atom * BlendWeight
	* Rotation is accumulated multiplicatively (Rotation = SourceAtom.Rotation * Rotation).
	* Translation is accumulated additively (Translation += SourceAtom.Translation)
	* Scale is accumulated multiplicatively (Scale *= SourceAtom.Scale)
	*
	* Note: Rotation will not be normalized! Will have to be done manually.
	*
	* @param Atom The other transform to accumulate into this one
	* @param BlendWeight The weight to multiply Atom by before it is accumulated.
	*/
	inline void Accumulate(const VTransform& Atom, float BlendWeight)
	{
		//VTransform SourceAtom(Atom * BlendWeight);

		//// Add ref pose relative animation to base animation, only if rotation is significant.
		//if( FMath::Square(SourceAtom.Rotation.w) < 1.f - DELTA * DELTA )
		//{
		//	Rotation = SourceAtom.Rotation * Rotation;
		//}

		//Translation += SourceAtom.Translation;
		//Scale *= SourceAtom.Scale;

		//DiagnosticCheckNaN_All();
	}

	/**
	 * Set the translation and Scale components of this atom to a linearly interpolated combination of two other atoms
	 *
	 * Translation = FMath::Lerp(SourceAtom1.Translation, SourceAtom2.Translation, Alpha)
	 * Scale = FMath::Lerp(SourceAtom1.Scale, SourceAtom2.Scale, Alpha)
	 *
	 * @param SourceAtom1 The starting point source atom (used 100% if Alpha is 0)
	 * @param SourceAtom2 The ending point source atom (used 100% if Alpha is 1)
	 * @param Alpha The blending weight between SourceAtom1 and SourceAtom2
	 */
	//inline void LerpTranslationScale3D(const VTransform& SourceAtom1, const VTransform& SourceAtom2, ScalarRegister Alpha)
	//{
	//	Translation	= FMath::Lerp(SourceAtom1.Translation, SourceAtom2.Translation, Alpha);
	//	Scale = FMath::Lerp(SourceAtom1.Scale, SourceAtom2.Scale, Alpha);
	//}

	/**
	 * Accumulates another transform with this one
	 *
	 * Rotation is accumulated multiplicatively (Rotation = SourceAtom.Rotation * Rotation)
	 * Translation is accumulated additively (Translation += SourceAtom.Translation)
	 * Scale is accumulated additively (Scale += SourceAtom.Scale)
	 *
	 * @param SourceAtom The other transform to accumulate into this one
	 */
	inline void AccumulateWithAdditiveScale3D(const VTransform& SourceAtom)
	{
		//if( FMath::Square(SourceAtom.Rotation.w) < 1.f - DELTA * DELTA )
		//{
		//	Rotation = SourceAtom.Rotation * Rotation;
		//}

		//Translation += SourceAtom.Translation;
		//Scale += SourceAtom.Scale;

		//DiagnosticCheckNaN_All();
	}

	/**
	 * Normalize the rotation component of this transformation
	 */
	inline void NormalizeRotation()
	{
		Rotation.Normalize();
	}

	/**
	 * Checks whether the rotation component is normalized or not
	 *
	 * @return true if the rotation component is normalized, and false otherwise.
	 */
	//inline bool IsRotationNormalized() const
	//{
	//	return Rotation.IsNormalized();
	//}

	/**
	 * Blends the Identity atom with a weighted source atom and accumulates that into a destination atom
	 *
	 * SourceAtom = Blend(Identity, SourceAtom, BlendWeight)
	 * FinalAtom.Rotation = SourceAtom.Rotation * FinalAtom.Rotation
	 * FinalAtom.Translation += SourceAtom.Translation
	 * FinalAtom.Scale *= SourceAtom.Scale
	 *
	 * @param FinalAtom [in/out] The atom to accumulate the blended source atom into
	 * @param SourceAtom The target transformation (used when BlendWeight = 1); this is modified during the process
	 * @param BlendWeight The blend weight between Identity and SourceAtom
	 */
	inline static void BlendFromIdentityAndAccumulate(VTransform& FinalAtom, VTransform& SourceAtom, float BlendWeight)
	{
		//const VTransform IdentityAtom = VTransform::Identity;

		//// Scale delta by weight
		//if( BlendWeight < (1.f - ZERO_ANIMWEIGHT_THRESH) )
		//{
		//	SourceAtom.Blend(IdentityAtom, SourceAtom, BlendWeight);
		//}

		//// Add ref pose relative animation to base animation, only if rotation is significant.
		//if( FMath::Square(SourceAtom.Rotation.w) < 1.f - DELTA * DELTA )
		//{
		//	FinalAtom.Rotation = SourceAtom.Rotation * FinalAtom.Rotation;
		//}

		//FinalAtom.Translation += SourceAtom.Translation;
		//FinalAtom.Scale *= SourceAtom.Scale;

		//FinalAtom.DiagnosticCheckNaN_All();

		//checkSlow( FinalAtom.IsRotationNormalized() );
	}


	inline void SetFromMatrix(const VMatrix& Matrix)
	{
		Matrix.DecomposeFast(Scale, Rotation, Translation);
	}
};

// this function is from matrix, and all it does is to normalize rotation portion
inline void VTransform::RemoveScaling()
{
	Scale.Set(1,1,1);
	Rotation.Normalize();
}

/** Returns Multiplied Transform of 2 FTransforms **/
inline void VTransform::Multiply(VTransform * OutTransform, const VTransform * A, const VTransform * B)
{
	//	When Q = quaternion, S = single scalar scale, and T = translation
	//	QST(A) = Q(A), S(A), T(A), and QST(B) = Q(B), S(B), T(B)
	//	QST (AxB) 
	// QST(A) = Q(A)*S(A)*P*-Q(A) + T(A)
	// QST(AxB) = Q(B)*S(B)*QST(A)*-Q(B) + T(B)
	// QST(AxB) = Q(B)*S(B)*[Q(A)*S(A)*P*-Q(A) + T(A)]*-Q(B) + T(B)
	// QST(AxB) = Q(B)*S(B)*Q(A)*S(A)*P*-Q(A)*-Q(B) + Q(B)*S(B)*T(A)*-Q(B) + T(B)
	// QST(AxB) = [Q(B)*Q(A)]*[S(B)*S(A)]*P*-[Q(B)*Q(A)] + Q(B)*S(B)*T(A)*-Q(B) + T(B)

	//	Q(AxB) = Q(B)*Q(A)
	//	S(AxB) = S(A)*S(B)
	//	T(AxB) = Q(B)*S(B)*T(A)*-Q(B) + T(B)
	VTransform Ret;
	Ret.Rotation = B->Rotation*A->Rotation;
	Ret.Scale = A->Scale*B->Scale;
	Ret.Translation = B->Rotation.Rotate(B->Scale*A->Translation) + B->Translation;
	*OutTransform = Ret;
}


inline VTransform VTransform::GetRelativeTransform(const VTransform& Other) const
{
	// A * B(-1) = VQS(B)(-1) (VQS (A))
	// 
	// Scale = S(A)/S(B)
	// Rotation = Q(B)(-1) * Q(A)
	// Translation = 1/S(B) *[Q(B)(-1)*(T(A)-T(B))*Q(B)]
	// where A = this, B = Other
	VTransform Result;

	VASSERT(IsValidScale(Other.Scale));
	VASSERT(IsValidScale(Scale));
	const VVector3 InvScale(1.0f/Other.Scale.x, 1.0f/Other.Scale.y, 1.0f/Other.Scale.z);
	Result.Scale = Scale * InvScale;
	
	VQuat InvRot(-Other.Rotation);
	Result.Rotation = InvRot*Rotation;

	Result.Translation = (InvRot.Rotate(Translation - Other.Translation))*InvScale;

	return Result;
}

/** 
 * Apply Scale to this transform
 */
inline VTransform VTransform::GetScaled(float InScale) const
{
	VTransform A(*this);
	A.Scale *= InScale;
	return A;
}

/** 
 * Apply Scale to this transform
 */
inline VTransform VTransform::GetScaled(VVector3 InScale) const
{
	VTransform A(*this);
	A.Scale *= InScale;

	return A;
}

/** Transform homogenous VVector4, ignoring the scaling part of this transform **/
inline VVector4 VTransform::TransformFVector4NoScale(const VVector4& V) const
{
	//Transform using QST is following
	//QST(P) = Q*S*P*-Q + T where Q = quaternion, S = scale, T = translation
	VVector4 Transform = Rotation.Rotate(V);
	if (V.w == 1.f)
	{
		Transform += VVector4(Translation, 1.f);
	}

	return Transform;
}

/** Transform VVector4 **/
inline VVector4 VTransform::TransformFVector4(const VVector4& V) const
{
	// if not, this won't work
	//checkSlow (V.w == 0.f || V.w == 1.f);

	//Transform using QST is following
	//QST(P) = Q*S*P*-Q + T where Q = quaternion, S = scale, T = translation

	VVector3 V3(V);
	V3 = (Rotation.Rotate(V3*Scale));
	if (V.w == 1.f)
	{
		V3 += Translation;
	}

	return VVector4(V3,V.w);
}



inline VVector3 VTransform::TransformPosition(const VVector3& V) const
{
	VVector3 Ret;
	Ret = (Rotation.Rotate(V*Scale));
	Ret += Translation;
	return Ret;
}

inline VVector3 VTransform::TransformVector(const VVector3& V) const
{
	VVector3 Ret;
	Ret = (Rotation.Rotate(V*Scale));
	return Ret;
}

inline VVector3 VTransform::TransformVectorNoScale(const VVector3& V) const
{
	//return TransformFVector4NoScale(VVector4(V.x,V.y,V.z,0.0f));
	return VVec3_0;
}

// do backward operation when inverse, translation -> rotation -> scale
inline VVector3 VTransform::InverseTransformPosition(const VVector3 &V) const
{
	return VVec3_0;
//	return ( Rotation.Inverse() * (V-Translation) ) * GetSafeScaleReciprocal(Scale);
}

// do backward operation when inverse, translation -> rotation
inline VVector3 VTransform::InverseTransformPositionNoScale(const VVector3 &V) const
{
	return VVec3_0;
//	return ( Rotation.Inverse() * (V-Translation) );
}


// do backward operation when inverse, translation -> rotation -> scale
inline VVector3 VTransform::InverseTransformVector(const VVector3 &V) const
{
	return VVec3_0;
	//return ( Rotation.Inverse() * V ) * GetSafeScaleReciprocal(Scale);
}

// do backward operation when inverse, translation -> rotation
inline VVector3 VTransform::InverseTransformVectorNoScale(const VVector3 &V) const
{
	return VVec3_0;
	//return ( Rotation.Inverse() * V );
}

inline VTransform VTransform::operator*(const VTransform& Other) const
{
	VTransform Output;
	Multiply(&Output, this, &Other);
	return Output;
}

inline void VTransform::operator*=(const VTransform& Other)
{
	Multiply(this, this, &Other);
}

inline VTransform VTransform::operator*(const VQuat& Other) const
{
	VTransform Output, OtherTransform(Other);
	Multiply(&Output, this, &OtherTransform);
	return Output;
}

inline void VTransform::operator*=(const VQuat& Other)
{
	VTransform OtherTransform(Other);
	Multiply(this, this, &OtherTransform);
}


#endif 