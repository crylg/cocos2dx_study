#pragma once
#ifndef __VSTREAM_H__
#define __VSTREAM_H__

#include "VString.h"

class VStream
{
public:
	// Close The File
	virtual void Close() = 0;
	// Get Stream Size
	virtual UINT Size() = 0;
	// Seek to Position
	virtual BOOL Seek(UINT Pos) = 0;
	// Read and return Num Readed Bytes
	virtual UINT Read(void* Dest, UINT Size)  = 0;
	// Write and return num writed bytes
	virtual UINT Write(const void* Dest, UINT Size) = 0;
	// tell me the position offset
	virtual UINT Tell() const = 0;
	// flush 
	virtual void Flush() = 0;

	//
	template <typename T> 
	inline void Write(const T& value) 
	{
#if VDEBUG
		VASSERT(Write((BYTE *)&value, sizeof(value)) == sizeof(value));
#else
		Write((BYTE *)&value, sizeof(value));
#endif 
	}

	// stream like operators for storing data
	inline VStream& operator<<(bool value) 
	{
		Write<char>((char)value);
		return *this;
	}
	// unsigned
	inline VStream &operator<<(BYTE value) {
		Write<BYTE>(value);
		return *this;
	}
	inline VStream &operator<<(WORD value) {
		Write<WORD>(value);
		return *this;
	}
	inline VStream &operator<<(UINT value) {
		Write<UINT>(value);
		return *this;
	}
	inline VStream &operator<<(UINT64 value) {
		Write<UINT64>(value);
		return *this;
	}
	inline VStream &operator<<(char value) {
		Write<char>(value);
		return *this;
	}
	inline VStream &operator<<(short value) {
		Write<SHORT>(value);
		return *this;
	}
	inline VStream &operator<<(int value) {
		Write<int>(value);
		return *this;
	}
	inline VStream &operator<<(INT64 value) {
		Write<INT64>(value);
		return *this;
	}
	inline VStream &operator<<(float value)
	{
		Write<float>(value);
		return *this;
	}
	inline VStream &operator<<(double value) 
	{
		Write<double>(value);
		return *this;
	}
	inline VStream &operator<<(const VString& value) 
	{
		Write((UINT)value.Length());
		if (value.Length())
		{
			Write((BYTE*)value.Str(), value.Length());
		}
		return *this;
	}
	inline VStream &operator<<(const char *str) 
	{
		UINT len = strlen(str);
		Write(len);
		Write((BYTE*)str, len);
		return *this;
	}

	// stream like operators for reading data
	template<typename T>
	inline T Read() 
	{
		T Ret;
#if VDEBUG
		VASSERT(Read(&Ret, sizeof(T)) == sizeof(T));
#else 
		Read(&Ret, sizeof(T));
#endif 
		return Ret;
	}

	inline VStream &operator>>(bool& value) {
		value = Read<char>() > 0 ? true : false;
		return *this;
	}
	//unsigned
	inline VStream &operator>>(BYTE &value) {
		value = Read<BYTE>();
		return *this;
	}
	inline VStream &operator>>(WORD &value) {
		value = Read<WORD>();
		return *this;
	}
	inline VStream &operator>>(UINT &value) {
		value = Read<UINT>();
		return *this;
	}
	inline VStream &operator>>(UINT64 &value) {
		value = Read<UINT64>();
		return *this;
	}
	inline VStream &operator>>(char &value) {
		value = Read<char>();
		return *this;
	}
	inline VStream &operator>>(short &value) {
		value = Read<SHORT>();
		return *this;
	}
	inline VStream &operator>>(int &value) {
		value = Read<int>();
		return *this;
	}
	inline VStream &operator>>(INT64 &value) {
		value = Read<INT64>();
		return *this;
	}
	inline VStream &operator>>(float &value) {
		value = Read<float>();
		return *this;
	}
	inline VStream &operator>>(double &value) {
		value = Read<double>();
		return *this;
	}
	inline VStream &operator>>(VString& value) 
	{
		value.Clear();
		UINT len = Read<UINT>();
		if (len)
		{
			value.Reserve(len+1);
			value[len] = 0;
			Read(&value[0], len);
		}
		return *this;
	}
};


class VMemoryStream : public VStream
{

};




#endif 