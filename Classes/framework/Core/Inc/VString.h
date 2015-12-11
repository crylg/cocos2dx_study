/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2014 Venus.PK, All Right Reversed.
//	VString.h
//	Created: 2014-2-12   10:56
//
**********************************************************************************/
#ifndef __VSTRING_H__
#define __VSTRING_H__
#include "VArray.h"


inline int VStrUtf8ToUnicode(const char* UTF8, UINT& Unicode)
{

}


//std::string
class VCoreApi VString : public VPODArray<char>
{
public:
	inline VString():VPODArray<char>(0){}
	inline VString(const VString &s):VPODArray<char>(s.m_Size)
	{
		if (m_Size)
		{
			VMemcpy(m_Elements, s.m_Elements, m_Size);
		}
	}
	explicit inline VString(const char *s):VPODArray<char>(0)
	{
		if (s && s[0])
		{
			int len = strlen(s) + 1;
			Reserve(len);
			m_Size = len;
			VMemcpy(m_Elements, s, m_Size);
		}
	}
	inline VString(const char *s, UINT Count):VPODArray<char>(0)
	{
		if (Count)
		{
			Reserve(Count + 1);
			m_Size = Count + 1;
			VMemcpy(m_Elements, s, Count);
			m_Elements[Count] = 0;
		}
	}
	inline ~VString() {}

	inline VString& operator=(const char* s)
	{
		if (m_Elements != s)
		{
			if (s)
			{
				int len = strlen(s) + 1;
				Reserve(len);
				m_Size = len;
				VMemcpy(m_Elements, m_Capacity, s, len);
			}else
			{
				Clear();
			}
		}
		return *this;
	}


	inline VString& operator=(const VString& s)
	{
		if (this != &s)
		{
			Reserve(s.m_Size);
			m_Size = s.m_Size;
			if (m_Size)
			{
				VMemcpy(m_Elements, m_Capacity, s.m_Elements, m_Size);
			}
		}
		return *this;
	}

	inline VString& operator+=(char c)
	{
		if (c != '\0')
		{
			UINT Index;
			if (m_Size)
			{
				Index = m_Size -1;
				Increment(1);
			}else
			{
				Index = 0;
				Increment(2);
			}
			m_Elements[Index] = c;
			m_Elements[Index+1] = 0;
		}
		
		return *this;
	}

	inline VString& operator+=(const VString& s)
	{
		if (s.m_Size)
		{
			UINT Index;
			if (m_Size)
			{
				Index = m_Size - 1;
				Increment(s.m_Size - 1);
			}else
			{
				Index = 0;
				Increment(s.m_Size);
			}
			VMemcpy(m_Elements + Index, s.m_Elements, s.m_Size);
		}
		return *this;
	}

	inline VString& operator+=(const char* s)
	{
		if (s && *s)
		{
			UINT Len = strlen(s) + 1;
			UINT Index;
			if (m_Size)
			{
				Index = m_Size - 1;
				Increment(Len - 1);
			}else
			{
				Index = 0;
				Increment(Len);
			}
			VMemcpy(m_Elements + Index, s, Len);
		}
		return *this;
	}

	inline VString operator+(char c) const
	{
		return VString(*this) += c;
	}

	inline VString operator+(const char* s) const 
	{
		return VString(*this) += s;
	}

	inline VString operator+(const VString& s) const
	{
		return VString(*this) += s;
	}

	//inline operator const char*() const 
	//{
	//	return m_Size ? m_Elements : NULL;
	//}
	inline char& operator[](UINT index)		{	return m_Elements[index];	}
	inline const char& operator[](UINT index) const {	return m_Elements[index];	}
	inline UINT Length() const { return m_Size ? m_Size - 1 : 0; }
	inline BOOL Empty() const { return m_Size <= 1; }
	inline const char* Str() const 
	{
		return m_Size ? m_Elements : NULL;
	}

	static VString SubStr(const char* String, int Pos, int Count = 0xFFFFFFFF);
	VString SubStr(int Pos, int Count = 0xFFFFFFFF) const;



	VString Replace(const char* From, const char* To, BOOL IgnoreCase = FALSE) const;
	static VString Replace(const char* String, const char* From, const char* To, BOOL IgnoreCace = FALSE);

	/**
	 *  快速替换, From 的长度必须 >= To 的长度. 
	 */
	int ReplaceFast(const char* From, const char* To, BOOL IgnoreCase = FALSE);

	// Fast split .
	// 仅仅返回各个子字符串的开始和结束位置
	struct SplitResult
	{
		const char*	Start;
		const char*	End;
	};
	static UINT Split(const char* String, const char* Delimiters, VPODArray<SplitResult>& SpliteResults);
	static VArray<VString> Split(const char* String, const char* Delimiters);
	

	inline void Lower()
	{
		for (UINT Index = 0; Index < m_Size; ++Index)
		{
			m_Elements[Index] = VToLower(m_Elements[Index]);
		}
	}

	inline void Upper()
	{
		for (UINT Index = 0; Index < m_Size; ++Index)
		{
			m_Elements[Index] = VToUpper(m_Elements[Index]);
		}
	}

	inline int Find( const char Char) const 
	{
		return (int)VPODArray<char>::Find(Char);
	}

	inline int RFind(const char Char) const 
	{
		for (int Index = (int)(m_Size - 1); Index >= 0; --Index)
		{
			if (m_Elements[Index] == Char)
			{
				return Index;
			}
		}
		return -1;
	}

	//VString Trim(const char* Symbols) const;

	//static StringArray<> split(const char *str,const char *delimiters);


	/*explicit inline VString(const char *s):m_Str(0),m_Size(0),m_Capacity(0)
	{
		if (s && s[0])
		{

		}
		
		int len = strlen(s)
	}
	inline ~VString();

	VString& operator=(const char *s);
	VString& operator=(const VString &s);

	VString& operator+=(char c);
	VString& operator+=(const char *s);
	VString& operator+=(const VString &s);

	inline const char* c_str() const { return m_Str; }
	inline operator const char*() const { return m_Str; }

	inline char& operator[](UINT index)		{	return m_Str[index];	}
	inline const char& operator[](int index) const {	return m_Str[index];	}

	inline int size() const { return m_Size; }
	inline bool empty() const { return (m_Size == 0); }

	void resize(int size);
	void allocate(int size);
	void reserve(int size);

	void clear();
	void destroy();

	int find(char c) const;
	int find(const char *s) const;
	int find(const VString &s) const;

	int rfind(char c) const;
	int rfind(const char *s) const;
	int rfind(const VString &s) const;

	void copy(const char *s,int len = -1);
	void copy(const VString &s,int len = -1);

	void append(char c);
	void append(const char *s,int len = -1);
	void append(const VString &s,int len = -1);

	void append(int pos,char c);
	void append(int pos,const char *s,int len = -1);
	void append(int pos,const VString &s,int len = -1);

	void remove();
	void remove(int pos,int len = 1);

	void lower();
	void upper();

	void vprintf(const char *format,va_list argptr);
	void printf(const char *format,...);

	int vscanf(const char *format,va_list argptr);
	int scanf(const char *format,...);

	static int isdigit(int code);
	static int islower(int code);
	static int isupper(int code);
	static int isalpha(int code);

	static int atoi(const char *str);
	static long long atol(const char *str);
	static float atof(const char *str);
	static double atod(const char *str);


	static VString lower(const char *str);
	static VString upper(const char *str);

	static VString format(const char *format,va_list argptr);
	static VString format(const char *format,...);

	static int vsscanf(const char *str,const char *format,va_list argptr);
	static int sscanf(const char *str,const char *format,...);

	static VString trim(const char *str,const char *symbols = 0);
	static VString replace(const char *str,const char *before,const char *after);

	static UINT split(const char *str,const char *delimiters);
	static VString substr(const char *str,int pos,int len = -1);
	static VString addslashes(const char *str);
	static VString stripslashes(const char *str);

	static VString dirname(const char *str);
	static VString basename(const char *str);
	static VString pathname(const char *str);

	static VString absname(const char *path,const char *str);
	static VString relname(const char *path,const char *str);
	static VString extension(const char *str,const char *ext);

	static VString unicodeToUtf8(const wchar_t *src);
	static VString unicodeToUtf8(const unsigned int *src);
	static int utf8ToUnicode(const char *src,unsigned int &code);
	static int utf8ToUnicode(const char *src,wchar_t *dest,int size);
	static int utf8ToUnicode(const char *src,unsigned int *dest,int size);

	static int utf8strlen(const char *str);
	static VString utf8substr(const char *str,int pos,int len = -1);

	static int match(const char *pattern,const char *str);

	static const VString null;

	inline void Reserve(UINT NewSize)
	{
		if (NewSize > m_Capacity)
		{
			m_Capacity = m_Allocator.CalcCapacity(NewSize, m_Capacity);
			_Realloc(m_Capacity);
		}
	}*/
protected:
};

#define DECLARE_STRING_COMPARE(OP) \
	inline bool operator OP(const VString &s0,const VString &s1) { \
		return (strcmp(s0.Str(),s1.Str()) OP 0); \
	} \
	inline bool operator OP(const VString &s0, const char *s1) { \
		if(s1 == NULL) return 0; \
		return (strcmp(s0.Str(),s1) OP 0); \
	} \
	inline bool operator OP(const char *s0,const VString &s1) { \
		if(s0 == NULL) return 0; \
		return (strcmp(s0,s1.Str()) OP 0); \
	}

DECLARE_STRING_COMPARE(==)
DECLARE_STRING_COMPARE(!=)
DECLARE_STRING_COMPARE(<)
DECLARE_STRING_COMPARE(>)
DECLARE_STRING_COMPARE(<=)
DECLARE_STRING_COMPARE(>=)

#undef DECLARE_STRING_COMPARE

typedef VPODArray<VString::SplitResult> VStringSplitResults;



class VStackString
{
public:


protected:
	char m_Stack[256];
	char* m_String;
	UINT m_Size;
};

// 引用字符串
// 引用字符串按照其内容(不区分大小写)将会引用相同的包含字符串内容的地址. 
// 以后的比较中, 仅仅只需要判断其引用的地址是否相同即可.
// Note : RefString 大小写无关. \\ 与 / 相同.
// VRefString RefStr1("DaTa\\Some.Ext");
// VRefString RefStr2("data/some.ext");
// RefStr1 == RefStr2 return true!
// 
class VRefString
{
public:
	enum 
	{
		LENGTH_MAX = 8192,			// RefString 支持的最大字符长度. 超过此长度将会导致AllocReference失败.
	};

	VCoreApi static const char* AllocReference(const char* Str);
	VCoreApi static void DumpInfo();
	VCoreApi static UINT MemUsage();
	inline VRefString():m_RefPtr(NULL){}
	inline VRefString(const char* Str){ m_RefPtr = AllocReference(Str);}
	inline VRefString(const VRefString& StrRef):m_RefPtr(StrRef.m_RefPtr){}
	inline ~VRefString(){}

	inline void operator=( const VRefString& other )
	{
		m_RefPtr = other.m_RefPtr;
	}

	inline void operator=( const char* cStr )
	{
		if (cStr)
		{
			m_RefPtr = AllocReference(cStr);
		}else
		{
			m_RefPtr = NULL;
		}
	}
	inline BOOL operator !() const 
	{
		return m_RefPtr == NULL;
	};
	inline operator const char*() const	{	return m_RefPtr; }
	inline const char* CStr() const { return m_RefPtr;}
	// for map hash.
	inline bool operator<( const VRefString& rhs) const
	{
		return m_RefPtr < rhs.m_RefPtr;
	}
	friend inline BOOL operator==(const VRefString& s1, const VRefString& s2)
	{
		return s1.m_RefPtr == s2.m_RefPtr;
	}
	friend inline BOOL operator!=(const VRefString& s1, const VRefString& s2)
	{
		return s1.m_RefPtr != s2.m_RefPtr;
	}

	friend BOOL operator==(const VRefString& s1, const char* s2);
	friend BOOL operator!=(const VRefString& s1, const char* s2);

	friend BOOL operator==(const char* s1, const VRefString& s2);
	friend BOOL operator!=(const char* s1, const VRefString& s2);
private:
	const char* m_RefPtr;
};

typedef VRefString VName;

#endif 
