#ifndef __VARRAY_H__
#define __VARRAY_H__
#pragma once
#include "VTemplate.h"
#include "VByteStream.h"

// dynamic array for plain old data.
template<typename T, typename Allocator = vtpl::AllocatorCommon<T> >
class VArrayBase
{
	enum
	{
		GROW_MIN = 8,		
		GROW_MAX = 512,
	};
protected:
	UINT	m_Size;
	UINT	m_Capacity;
	T*		m_Elements;
	typedef Allocator ArrayAllocator;
	ArrayAllocator m_Allocator;
public: 
	inline VArrayBase(UINT InitSize = 0):m_Elements(NULL),m_Size(0),m_Capacity(0)
	{
		if(InitSize)
		{
			Reserve(InitSize);
			_Construct(0, InitSize);
			m_Size = InitSize;
		}
	}
 
	inline VArrayBase(const VArrayBase& p):m_Elements(NULL),m_Size(0),m_Capacity(0)
	{
		Reserve(p.m_Size);
		if (p.m_Size)
		{
			m_Size = p.m_Size;
			_Construct(0, m_Size);
			for (UINT i = 0; i < m_Size; i++)
			{
				m_Elements[i] = p[i];
			}
		}
	}

	inline ~VArrayBase()
	{
		Free();
	}

	inline UINT GetMemUsage() const	{ return GetCapacity() * sizeof(T);	}
	inline T* GetData()				{ return m_Elements;}
	inline const T* GetData() const	{ return m_Elements;}
	inline UINT Size() const		{ return m_Size;}
	inline BOOL IsEmpty() const		{ return (m_Size == 0); }
	inline void PushFront(const T& E)	{	InsertAt(0, E, 1);} 
	inline void PushBack(const T& E)	
	{	
		Reserve(m_Size+1);
		_Construct(m_Size, 1);
		m_Elements[m_Size++] = E;
	}
	inline void PopFront()				{	EraseAt(0);}
	inline void PopBack()				{	EraseAt(m_Size -1); }
	inline T& operator[](UINT index)	{	VASSERT(index < m_Size); return m_Elements[index]; }
	inline const T& operator[](UINT index) const	{	VASSERT(index < m_Size); return m_Elements[index]; }
	inline T& GetFirst()				{ VASSERT(m_Elements);return m_Elements[0]; } 
	inline const T& GetFirst() const	{ VASSERT(m_Elements);return m_Elements[0]; } 
	inline T& GetLast()					{ VASSERT(m_Size); return m_Elements[m_Size-1]; }
	inline const T& GetLast() const		{ VASSERT(m_Size); return m_Elements[m_Size-1]; } 
	inline UINT GetCapacity() const		{ return m_Capacity;}
	
	inline const T* begin() const { return m_Elements;  }
	inline const T* end() const { return m_Elements + m_Size; }
	// 
	inline void Clear()					{ m_Size = 0; }

	// Ïú»ÙÄÚ´æ
	inline void Free()	
	{
		if (m_Elements)
		{
			_Destruct(0, m_Size);
			_Realloc(0);
			m_Elements = NULL;
		}
		m_Size = m_Capacity = 0;
	}

	inline void Compact()
	{
		if (m_Capacity > m_Size)
		{
			_Realloc(m_Size);
			m_Capacity = m_Size; 
		}
	}

	inline void Append(const VArrayBase& Other)
	{	
		VASSERT(this != &Other); 
		if (Other.IsEmpty())
		{
			return;
		}
		Reserve(m_Size + Other.m_Size);
		_Construct(m_Size, Other.m_Size);
		for (UINT i = 0; i < Other.m_Size; i++)
		{
			m_Elements[m_Size + i] = Other[i];
		}
		m_Size += Other.m_Size;
	}
	inline void Reserve(UINT NewSize)
	{
		if (NewSize > m_Capacity)
		{
			m_Capacity = m_Allocator.CalcCapacity(NewSize, m_Capacity);
			_Realloc(m_Capacity);
		}
	}

	inline void InsertAt(UINT Index, const T& NewElement, UINT Count = 1)
	{
		VASSERT(Count > 0);
		VASSERT(Index <= m_Size);
		UINT NewSize = m_Size + Count;
		// reserve memory.
		Reserve(NewSize);
		// move prev data that after Index.
		if (Index < m_Size)
		{
			VMemmove(m_Elements + Index + Count, m_Elements + Index, (m_Size - Index) * sizeof(T));
		}

		// construct the new data
		_Construct(Index, Count);
		while(Count--)
		{
			m_Elements[Index++] = NewElement;
		}
		m_Size = NewSize;
	}

	inline void EraseAt(UINT Index, UINT Count = 1)
	{
		VASSERT(Index + Count <= m_Size);
		VASSERT(Count > 0);
		_Destruct(Index, Count);
		UINT MoveCount = m_Size - (Index + Count);
		if (MoveCount)
		{
			VMemmove(m_Elements + Index, m_Elements + Index + Count, MoveCount * sizeof(T));
		}
		m_Size -= Count;
	}
	
	// FOR POD
	inline UINT Increment(UINT Count)
	{
		Reserve(m_Size + Count);
		m_Size += Count;
		return m_Size;
	}

	inline void Increment(UINT Count, const T& Value)
	{
		Reserve(m_Size + Count);
		UINT Start = m_Size;
		m_Size += Count;
		for (UINT Index = Start; Index < m_Size; ++Index)
		{
			m_Elements[Index] = Value;
		}
	}


	inline BOOL Remove(const T& Value)							
	{ 
		UINT index = Find(Value); 
		if (index == -1) 
			return FALSE; 
		EraseAt(index,1); 
		return TRUE; 
	}

	inline BOOL Contains(const T& Value) const							
	{ 
		return (Find(Value) != (UINT)-1); 
	}

	inline UINT Find(const T& Value) const								
	{ 
		for (UINT i=0; i<m_Size; ++i) 
		{ 
			if (m_Elements[i] == Value) 
				return i; 
		} 
		return (UINT)-1; 
	}

	inline VArrayBase& operator=(const VArrayBase& p)
	{
		Reserve(p.m_Size);
		m_Size = p.m_Size;
		_Construct(0, m_Size);
		if (m_Size)
		{
			for (UINT i = 0; i < m_Size; i++)
			{
				m_Elements[i] = p[i];
			}
		}
		return *this;
	}

	inline BOOL operator==(const VArrayBase& other) const				
	{ 
		if (m_Size != other.m_Size) 
			return FALSE; 
		for (UINT i = 0; i < m_Size; ++i) 
			if (m_Elements[i] != other.m_Elements[i]) 
				return FALSE; 
		return TRUE; 
	}

	inline VArrayBase operator+(const VArrayBase& other) const
	{ 
		VArrayBase NewArray(*this);
		NewArray.Append(other);
		return NewArray;
	}

	inline VArrayBase& operator+=(const T& other)					
	{ 
		PushBack(other);
		return *this; 
	}

	inline VArrayBase& operator+=(const VArrayBase& other)					
	{ 
		Append(other); 
		return *this; 
	}
	

private:
	inline void	_Construct(UINT Index, UINT Count)									
	{ 
		m_Allocator.ConstructElements(m_Elements, Index, Count);
	}

	inline void	_Destruct(UINT Index, UINT Count)									
	{ 
		m_Allocator.DestructElements(m_Elements, Index, Count);
	}
	
	inline void _Realloc(UINT NewSize)
	{
		m_Elements = m_Allocator.Resize(m_Elements, m_Size, NewSize);
	}
};

//////////////////////////////////////////////////////////////////////////
// VArrayBase Implemention
//////////////////////////////////////////////////////////////////////////

template<typename T, typename Allocator =  vtpl::AllocatorCommonPOD<T> >
class VPODArray : public VArrayBase<T, Allocator>
{
public:
	inline VPODArray(UINT InitSize = 0):VArrayBase<T, Allocator>(InitSize)
	{
	}

	inline VPODArray(const VPODArray& p):VArrayBase<T, Allocator>(p.m_Size)
	{
		if (this->m_Size)
		{
			VMemcpy(this->m_Elements, p.m_Elements, this->m_Size * sizeof(T));
		}
	}
	inline ~VPODArray()
	{
	}
	inline void EraseFast(UINT Index)
	{
		VASSERT(Index < this->m_Size);
		this->m_Elements[Index] = this->m_Elements[this->m_Size - 1];
		this->m_Size--;
	}

	friend inline VByteStream& operator<<(VByteStream& Stream, const VPODArray& Array)
	{
		Stream << Array.m_Size;
		if (Array.m_Size)
		{
			for (UINT i = 0; i < Array.m_Size; i++)
			{
				Stream << Array[i];
			}
		}
		return Stream;
	}
	friend inline VByteStream& operator>>(VByteStream& Stream, VPODArray& Array)
	{
		UINT Size;
		Stream >> Size;
		if (Size)
		{
			Array.Reserve(Size);
			Array.m_Size = Size;
			for (UINT i = 0; i < Size; i++)
			{
				Stream >> Array[i];
			}
		}
		return Stream;
	}
};

template<typename T, typename Allocator =  vtpl::AllocatorCommon<T> >
class VArray : public VArrayBase<T,Allocator>
{
public:
	inline VArray(UINT InitSize = 0):VArrayBase<T, Allocator>(InitSize)
	{
	}
	~VArray(void)
	{
	}
private:
	
};




#endif 
