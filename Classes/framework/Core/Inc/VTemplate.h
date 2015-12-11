#ifndef __VTEMPLATE_H__
#define __VTEMPLATE_H__
#pragma once
#include "VBase.h"

#include <memory>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <queue>
#include <stack>
#include <algorithm>
#include <functional>

#if VPLATFORM_WIN32
#include <hash_map>
#endif 

#include <sstream>
namespace vtpl{

	// stl allocator.
template<typename T>
class allocator
{	
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T  *pointer;
	typedef const T  *const_pointer;
	typedef T & reference;
	typedef const T & const_reference;
	typedef T value_type;
public:
	inline allocator()
	{	
	}
	inline allocator(const allocator<T>& rhs )
	{
	}
	template<typename _Other>
	inline allocator(const allocator<_Other>& rhs)
	{
	}
	template<typename _Other>
	inline allocator<T>& operator=(const allocator<_Other>& rhs)
	{
		return (*this);
	}
	inline pointer allocate(size_type _Count, const void *)
	{
		return allocate(_Count);
	}
	inline pointer allocate(size_type _Count)
	{
		return (pointer)VMalloc(_Count * sizeof(T) );
	}
	inline void deallocate(pointer _Ptr, size_type)
	{
		VFree(_Ptr);
	}
	inline void construct(pointer _Ptr, const T& _Val)
	{
		new ((void *)_Ptr) T(_Val);
	}
	inline void destroy(pointer _Ptr)
	{	
		(_Ptr)->~T();
	}
	inline size_t max_size() const
	{
		size_t _Count = (size_t)(-1) / sizeof (T);
		return (0 < _Count ? _Count : 1);
	}

	template<typename _Other>
	struct rebind
	{	// convert an allocator<T> to an allocator <_Other>
		typedef allocator<_Other> other;
	};

	inline pointer address(reference _Val) const
	{
		return (&_Val);
	}
	inline const_pointer address(const_reference _Val) const
	{	
		return (&_Val);
	}
	bool operator==(const allocator<T>&) {return true;}
	bool operator!=(const allocator<T>&) {return false;}
};

// 用于模板的通用内存分配
template<typename T>
class AllocatorCommon
{
public:
	inline UINT CalcCapacity(UINT Size, UINT PrevCapacity) const
	{
		UINT Grow = (Size >> 2); //+ (PrevCapacity >> 4);		// for some big container, capacity is too large!!!
		if (Grow < 16)
		{
			Grow = 16;
		}
		return Size + Grow;
	}

	inline void ConstructElements(T* Elements, UINT Index, UINT Count)
	{
		T* Element = Elements + Index;
		for (UINT ElementIndex = 0; ElementIndex < Count; ++ElementIndex)
		{
			::new(Element++) T;
		}
	}

	inline void DestructElements(T* Elements, UINT Index, UINT Count)
	{
		T* Element = Elements + Index;
		for (UINT ElementIndex = 0; ElementIndex < Count; ++ElementIndex)
		{
			Element->~T();
			++Element;
		}
	}

	inline T* Resize(T* Elements, UINT CurrSize, UINT NewSize)
	{
		if (NewSize || Elements)
		{
			return (T*)VRealloc(Elements, NewSize * sizeof(T));
		}
		return Elements;
	}

	inline T* Alloca(UINT _Count)
	{
		T* ptr = (T*)VMalloc(_Count * sizeof(T) );
		return ptr;
	}
	inline void Free(T* _Ptr)
	{
		VFree(_Ptr);
	}
};

template<typename T>
class AllocatorCommonPOD : public AllocatorCommon<T>
{
public:
	enum{
		GROW_MIN = 8,
		GROW_MAX = 512,
	};

	inline void ConstructElements(T* Elements, UINT Index, UINT Count)
	{
		// nothing todo with pod type
	}

	inline void DestructElements(T* Elements, UINT Index, UINT Count)
	{
		// nothing todo with pod type
	}

	inline T* Alloca(UINT _Count)
	{
		return (T*)VMalloc(_Count * sizeof(T) );
	}
	inline void Free(void* _Ptr)
	{
		VFree(_Ptr);
	}
};


template<VStackMemoryPool& StackPool, typename T>
class VStackPoolAllocator
{
public:
	inline UINT CalcCapacity(UINT Size, UINT PrevCapacity) const
	{
		UINT Grow = (Size >> 2); //+ (PrevCapacity >> 4);		// for some big container, capacity is too large!!!
		if (Grow < 16)
		{
			Grow = 16;
		}
		return Size + Grow;
	}

	inline void ConstructElements(T* Elements, UINT Index, UINT Count)
	{
		T* Element = Elements + Index;
		for (UINT ElementIndex = 0; ElementIndex < Count; ++ElementIndex)
		{
			::new(Element++) T;
		}
	}

	inline void DestructElements(T* Elements, UINT Index, UINT Count)
	{
		T* Element = Elements + Index;
		for (UINT ElementIndex = 0; ElementIndex < Count; ++ElementIndex)
		{
			Element->~T();
			++Element;
		}
	}

	inline T* Resize(T* Elements, UINT CurrSize, UINT NewSize)
	{
		if (NewSize)
		{
			T* NewElements = Alloca(NewSize * sizeof(T));
			if (Elements)
			{
				VMemcpy(NewElements, Elements, CurrSize * sizeof(T));
			}
			return NewElements;
		}
		return NULL;
	}

	inline T* Alloca(UINT _Count)
	{
		return (T*)StackPool.Alloc(_Count);
	}
	inline void Free(T* _Ptr)
	{
		
	}
};

template<VStackMemoryPool& StackPool, typename T>
class VStackPoolAllocatorPOD : public AllocatorCommonPOD<T>
{
public:
	inline T* Alloca(UINT _Count)
	{
		return (T*)StackPool.Alloc(_Count);
	}
	inline void Free(void* _Ptr)
	{
	}

	inline T* Resize(T* Elements, UINT CurrSize, UINT NewSize)
	{
		if (NewSize)
		{
			T* NewElements = Alloca(NewSize * sizeof(T));
			if (Elements)
			{
				VMemcpy(NewElements, Elements, CurrSize * sizeof(T));
			}
			return NewElements;
		}
		return NULL;
	}

};


#if VPLATFORM_WIN32
#define STD std
#elif VPLATFORM_ANDROID 
#define STD stlport				// ???
#endif 
   
#if 0 
//////////////////////////////////////////////////////////////////////
// searches the given entry in the map by key, and if there is none, returns the default value
#ifdef WIN64
#	include <map>
#	define hash_map map
template <typename Map, typename key_type, typename mapped_type>
inline mapped_type find_in_map(const Map& mapKeyToValue, key_type key, mapped_type valueDefault)
#else	// 32位
#	if defined(LINUX)
#		include "platform.h"
#		include <ext/hash_map>
#	else	// WIN32
#		include <hash_map>
#	endif
template <typename Map>
inline typename Map::mapped_type find_in_map(const Map& mapKeyToValue, typename Map::key_type key, typename Map::mapped_type valueDefault)
#endif
{
	typename Map::const_iterator it = mapKeyToValue.find (key);
	if (it == mapKeyToValue.end())
		return valueDefault;
	else
		return it->second;
}

#endif 

//////////////////////////////////////////////////////////////////////
// searches the given entry in the map by key, and if there is none, returns the default value
// The values are taken/returned in REFERENCEs rather than values
#ifdef WIN64
template <typename Map, typename key_type, typename mapped_type>
inline mapped_type& find_in_map_ref(Map& mapKeyToValue, key_type key, mapped_type& valueDefault)
#else
template <typename Map>
inline typename Map::mapped_type& find_in_map_ref(Map& mapKeyToValue, typename Map::key_type key, typename Map::mapped_type& valueDefault)
#endif
{
	typename Map::iterator it = mapKeyToValue.find (key);
	if (it == mapKeyToValue.end())
		return valueDefault;
	else
		return it->second;
}

//////////////////////////////////////////////////////////////////////
template <class Container>
inline unsigned sizeofArray (const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type)*arr.size());
}

//////////////////////////////////////////////////////////////////////
template <class Container>
unsigned sizeofVector (const Container& arr)
{
	return (unsigned)(sizeof(typename Container::value_type)*arr.capacity());
}

//////////////////////////////////////////////////////////////////////
template <class Container>
unsigned sizeofArray (const Container& arr, unsigned nSize)
{
	return arr.empty()?0u:(unsigned)(sizeof(typename Container::value_type)*nSize);
}

//////////////////////////////////////////////////////////////////////
template <class Container>
unsigned capacityofArray (const Container& arr)
{
	return (unsigned)(arr.capacity()*sizeof(arr[0]));
}

//////////////////////////////////////////////////////////////////////
template <class T>
unsigned countElements (const std::vector<T>& arrT, const T& x)
{
	unsigned nSum = 0;
	for (typename std::vector<T>::const_iterator iter = arrT.begin(); iter != arrT.end(); ++iter)
		if (x == *iter)
			++nSum;
	return nSum;
}



//////////////////////////////////////////////////////////////////////////
//! Fills vector with contents of map.
//////////////////////////////////////////////////////////////////////////
template <class Map,class Vector>
inline void map_to_vector( const Map& theMap,Vector &array )
{
	array.resize(0);
	array.reserve( theMap.size() );
	for (typename Map::const_iterator it = theMap.begin(); it != theMap.end(); ++it)
	{
		array.push_back( it->second );
	}
}

//////////////////////////////////////////////////////////////////////////
//! Fills vector with contents of set.
//////////////////////////////////////////////////////////////////////////
template <class Set,class Vector>
inline void set_to_vector( const Set& theSet,Vector &array )
{
	array.resize(0);
	array.reserve( theSet.size() );
	for (typename Set::const_iterator it = theSet.begin(); it != theSet.end(); ++it)
	{
		array.push_back( *it );
	}
}

//////////////////////////////////////////////////////////////////////////
//! Find and erase element from container.
// @return true if item was find and erased, false if item not found.
//////////////////////////////////////////////////////////////////////////
template <class Container,class Value>
inline bool find_and_erase( Container& container,const Value &value )
{
	typename Container::iterator it = std::find( container.begin(),container.end(),value );
	if (it != container.end())
	{
		container.erase( it );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//! Push back to container unique element.
// @return true if item added, false overwise.
template <class Container,class Value>
inline bool push_back_unique( Container& container,const Value &value )
{
	if (std::find(container.begin(),container.end(),value) == container.end())
	{
		container.push_back( value );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//! Find element in container.
// @return true if item found.
template <class Container,class Value>
inline bool find( Container& container,const Value &value )
{
	return std::find(container.begin(),container.end(),value) != container.end();
}

//////////////////////////////////////////////////////////////////////////
//! Convert arbitary class to const char*
//////////////////////////////////////////////////////////////////////////
template <class Type>
inline const char* LCastCStr( const Type &type )
{
	return type;
}

//! Specialization of string to const char cast.
template <>
inline const char* LCastCStr( const std::string &type )
{
	return type.c_str();
}

//////////////////////////////////////////////////////////////////////////
//! Case sensetive less key for any type convertable to const char*.
//////////////////////////////////////////////////////////////////////////
template <class Type>
struct less_strcmp : public std::binary_function<Type,Type,bool> 
{
	inline bool operator()( const Type &left,const Type &right ) const
	{
		return strcmp(LCastCStr(left),LCastCStr(right)) < 0;
	}
};

//////////////////////////////////////////////////////////////////////////
//! Case insensetive less key for any type convertable to const char*.
template <class Type>
struct less_stricmp : public std::binary_function<Type,Type,bool> 
{
	bool operator()( const Type &left,const Type &right ) const
	{
		return stricmp(LCastCStr(left),LCastCStr(right)) < 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// Hash map usage:
// typedef stl::hash_map<string,int, stl::hash_stricmp<string> > StringToIntHash;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//! Case sensetive string hash map compare structure.
//////////////////////////////////////////////////////////////////////////
template <class Key>
class hash_strcmp
{
public:
	enum {	// parameters for hash table
		bucket_size = 4,	// 0 < bucket_size
		min_buckets = 8	};// min_buckets = 2 ^^ N, 0 < N

		size_t operator()( const Key& key ) const
		{
			unsigned int h = 0; 
			const char *s = LCastCStr(key);
			for (; *s; ++s) h = 5*h + *(unsigned char*)s;
			return size_t(h);

		};
		bool operator()( const Key& key1,const Key& key2 ) const
		{
			return strcmp(LCastCStr(key1),LCastCStr(key2)) < 0;
		}
};

//////////////////////////////////////////////////////////////////////////
//! Case insensetive string hash map compare structure.
template <class Key>
class hash_stricmp
{
public:
	enum {	// parameters for hash table
		bucket_size = 4,	// 0 < bucket_size
		min_buckets = 8	};// min_buckets = 2 ^^ N, 0 < N

		size_t operator()( const Key& key ) const
		{
			unsigned int h = 0; 
			const char *s = LCastCStr(key);
			for (; *s; ++s) h = 5*h + tolower(*(unsigned char*)s);
			return size_t(h);

		};
		bool operator()( const Key& key1,const Key& key2 ) const
		{
			return stricmp(LCastCStr(key1),LCastCStr(key2)) < 0;
		}

};

template<typename T, typename A = allocator<T> >
class vector : public std::vector<T,A>
{
};

template<typename T, typename A = allocator<T> >
class list : public std::list<T, A>
{
};

template<typename T, typename A = allocator<T> >
class deque : public std::deque<T,A>
{

};

template<typename T, typename C = deque<T> >
class queue : public std::queue<T,C>
{

};

template<class K, class V, class C = std::less<K>, typename A = allocator<std::pair<const K, V> > >
class map : public std::map<K,V,C,A >
{
};

template<typename K, typename P = std::less<K>, typename A = allocator<K> >
class set : public std::set<K,P,A>
{
};

typedef std::basic_stringstream < char, std::char_traits<char>, allocator<char> > stringstream;


} // namespace vtpl


//
//	usage :
//   struct ListElement : public VFastLinkList
//	 {
//		int Member;
//	}
//
//	VFastLinkList* List = NULL;
//  ListElement l0; l0.Member = 1;
//


/**
 * Encapsulates a link in a single linked list with constant access time.
 */
template<class T> 
class VLinkList
{
public:
	VLinkList():
		NextLink(NULL),
		PrevLink(NULL)
	{}
	VLinkList(const T& InElement):
		Element(InElement),
		NextLink(NULL),
		PrevLink(NULL)
	{}

			/**
	 * Adds this element to a list, before the given element.
	 *
	 * @param Before	The link to insert this element before.
	 */
	void Link(VLinkList*& Before)
	{
		if(Before)
		{
			Before->PrevLink = &NextLink;
		}
		NextLink = Before;
		PrevLink = &Before;
		Before = this;
	}

	/**
	 * Removes this element from the list in constant time.
	 *
	 * This function is safe to call even if the element is not linked.
	 */
	void Unlink()
	{
		if( NextLink )
		{
			NextLink->PrevLink = PrevLink;
		}
		if( PrevLink )
		{
			*PrevLink = NextLink;
		}
		// Make it safe to call Unlink again.
		NextLink = NULL;
		PrevLink = NULL;
	}



	// Accessors.
	T& operator->()
	{
		return Element;
	}
	const T& operator->() const
	{
		return Element;
	}
	T& operator*()
	{
		return Element;
	}
	const T& operator*() const
	{
		return Element;
	}
	VLinkList* Next()
	{
		return NextLink;
	}
private:
	T Element;
	VLinkList* NextLink;
	VLinkList** PrevLink;
};


struct VFastLinkListNode
{
	inline VFastLinkListNode():Prev(NULL),Next(NULL)
	{
	}
	VFastLinkListNode** Prev;
	VFastLinkListNode* Next;

	// link to list head
	inline void Link(VFastLinkListNode*& List)
	{
		if(List)
		{
			List->Prev = &Next;
		}
		Next = List;
		Prev = &List;
		List = this;
	}

	inline void Unlink()
	{
		if(Next)
		{
			Next->Prev = Prev;
		}
		if( Prev)
		{
			*Prev = Next;
		}
		Next = NULL;
		Prev = NULL;
	}
};



#endif 
