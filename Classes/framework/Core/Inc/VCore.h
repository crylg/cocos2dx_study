#ifndef __VCORE_H__
#define __VCORE_H__

#include "VBase.h"
#include "VMath.h"
#include "VInterpolate.h"
#include "VTransform.h"
#include "VPackedFloat.h"
#include "VColor.h"
#include "VArray.h"
#include "VSmartPtr.h"
#include "VTemplate.h"
#include "VArray.h"
#include "VString.h"
#include "VThreading.h"
#include "VFile.h"
#include "VRTTI.h"

#include "VBounds.h"
#include "VGeometry.h"

#include "VTask.h"

// Main thread stack memory pool
VCoreApi extern VStackMemoryPool VMainStackPool;


/// Main stack pool array.
template<typename T>
class VMSPArray : public VArray<T,vtpl::VStackPoolAllocator<VMainStackPool, T> >
{
public:
	typedef VArray<T, vtpl::VStackPoolAllocator<VMainStackPool, T> > Supper;

	inline VMSPArray(UINT InitSize = 0) :Supper(InitSize)
	{
	}
};


VCoreApi extern BOOL VCoreInit();
VCoreApi extern void VCoreShutdown();
class VGuid;
VCoreApi VGuid VGenGuid(); 
VCoreApi VGuid VGenGuid(const char* Name);


// we use only 64 bits for guid.
// this guid mybe not global unique, you must check it yourself.
class VGuid
{
public:
	union
	{
		struct  
		{
			UINT A;
			UINT B;
		};
		UINT64 ID;
	};
	
	inline VGuid(){}
	inline VGuid(const VGuid& Other):ID(Other.ID){}
	inline VGuid( UINT inA, UINT inB): A(inA), B(inB){}
	
	inline BOOL IsValid() const{return (A | B ) != 0;}
	inline void Invalidate(){A = B = 0;}

	friend inline BOOL operator==(const VGuid& X, const VGuid& Y)
	{
		return ((X.A ^ Y.A) | (X.B ^ Y.B)) == 0;
	}
	friend inline BOOL operator!=(const VGuid& X, const VGuid& Y)
	{
		return ((X.A ^ Y.A) | (X.B ^ Y.B)) != 0;
	}
	
	// for map hash
	inline bool operator<(const VGuid& Guid) const 
	{
		return ID < Guid.ID;
	}
};

VCoreApi extern const VGuid VGUID_NULL;

VCoreApi UINT VCrc32(const void* Data, UINT Length, UINT CRC = 0);
VCoreApi UINT VStrCrc(const char* Str, UINT CRC = 0);
VCoreApi UINT VStriCrc(const char* Str);

// 字符串哈希(大小写无关)
//VCoreApi UINT LStrHash(const char* Str);


// 应用程序路径
VCoreApi const char* VAppPath();
VCoreApi const char* VDataPath();


// tinyxml2 wrapper

#include "tinyxml2.h"

typedef tinyxml2::XMLDocument VXMLDocument;
typedef tinyxml2::XMLElement VXMLElement;
typedef tinyxml2::XMLAttribute VXMLAttribute;
typedef tinyxml2::XMLComment VXMLComment;
typedef tinyxml2::XMLText VXMLText;
typedef tinyxml2::XMLDeclaration VXMLDeclaration;
typedef tinyxml2::XMLUnknown VXMLUnknown;
typedef tinyxml2::XMLPrinter VXMLPrinter;


inline VGuid VXML_LoadGuid(const VXMLElement* Element, const char* Attribute, const VGuid& Default = VGUID_NULL)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		VGuid Guid;
		if (VSscanf(sValue, "%u,%u", &Guid.A, &Guid.B) == 2)
		{
			return Guid;
		}
	}
	return Default;
}

inline bool VXML_Load(const VXMLElement* Element, const char* Attribute, VVector3& Vec3)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		if (VSscanf(sValue, "%f,%f,%f", &Vec3.x, &Vec3.y, &Vec3.z) == 3)
		{
			return true;
		}
	}
	return false;
}

inline bool VXML_Load(const VXMLElement* Element, const char* Attribute, VQuat& Quat)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		if (VSscanf(sValue, "%f,%f,%f,%f", &Quat.x, &Quat.y, &Quat.z, &Quat.w) == 4)
		{
			return true;
		}
	}
	return false;
}

inline VVector4 VXML_LoadVec4(const VXMLElement* Element, const char* Attribute, VVector4& Vec4)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		if (VSscanf(sValue, "%f,%f,%f,%f", &Vec4.x, &Vec4.y, &Vec4.z, &Vec4.w) == 4)
		{
			return Vec4;
		}
	}
	return Vec4;
}

inline VColor VXML_Load(const VXMLElement* Element, const char* Attribute, const VColorF& Color = VColorF::WHITE)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		VColor Ret;
		if (VSscanf(sValue, "%f,%f,%f,%f", &Ret.r, &Ret.g, &Ret.b, &Ret.a) == 4)
		{
			return Ret;
		}
	}
	return Color;
}

inline BOOL VXML_LoadBool(const VXMLElement* Element, const char* Attribute, BOOL Default = TRUE)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		int d = 0;
		if (VSscanf(sValue, "%d", &d) == 1)
		{
			return (BOOL)d;
		}
	}
	return Default;
}

inline float VXML_LoadFloat(const VXMLElement* Element, const char* Attribute, float Default = 0.0f)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		float d = 0;
		if (VSscanf(sValue, "%f", &d) == 1)
		{
			return (float)d;
		}
	}
	return Default;
}

inline int VXML_LoadInt(const VXMLElement* Element, const char* Attribute, int Default = 0)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		int d = 0;
		if (VSscanf(sValue, "%d", &d) == 1)
		{
			return d;
		}
	}
	return Default;
}

inline VString VXML_LoadString(const VXMLElement* Element, const char* Attribute)
{
	const char* sValue= Element->Attribute(Attribute);
	if (sValue && *sValue)
	{
		return VString(sValue);
	}
	return VString("");
}
#endif 