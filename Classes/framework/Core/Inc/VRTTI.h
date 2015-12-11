#pragma once
#ifndef __VRTTI_H__
#define __VRTTI_H__


union VAnyValue
{
	BYTE		ByteValue;
	BOOL		BoolValue;
	INT			IntValue;
	UINT		UIntValue;
	float		FloatValue;
	const char* StringValue;
	void*		PtrValue;
};

class VRTProperty
{
public:
	VRefString Name;

};

//class VCoreApi VObject 
//{
//
//};


class VCoreApi VClass
{
public:
	VRTProperty* FindProperty(const VRefString& Name) const { return NULL; }


protected:

};



#endif 
