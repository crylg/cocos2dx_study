/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2014 Venus.PK, All Right Reversed.
//	VSmartPtr.h
//	Created: 2007-10-3   10:36
//
**********************************************************************************/
#ifndef __VSMARTPTR_H__
#define __VSMARTPTR_H__
#pragma once
#include "VBase.h"

class VCoreApi VRefTarget
{
public:
	inline VRefTarget():m_RefCounter(0)
	{
	}
	virtual ~VRefTarget()
	{
		VASSERT(m_RefCounter == 0);
	}
	inline UINT AddRef() const	
	{ 
		return UINT(++m_RefCounter);
		//InterlockedIncrement((LONG*)&m_RefCounter);
	}
	inline UINT Release()
	{
		UINT RefCnt = UINT(--m_RefCounter);
		if(RefCnt == 0)
		{
			DeleteThis();
		}
		return RefCnt;

		//if(InterlockedDecrement((LONG*)&m_RefCounter) == 0)
	}
	inline UINT GetNumRef()	const	{ return m_RefCounter; }
protected:
	virtual void DeleteThis()  = 0;
	mutable UINT m_RefCounter;
};


// multi-thread reftarget
class VCoreApi VRefTargetMT
{
public:
	inline VRefTargetMT():m_RefCounter(0)
	{
	}
	virtual ~VRefTargetMT()
	{
		VASSERT(m_RefCounter == 0);
	}
	inline UINT AddRef() const	
	{ 
		return (UINT)VInterlockedIncrement(&m_RefCounter);
	}
	inline UINT Release()
	{
		int RefCnt = VInterlockedDecrement(&m_RefCounter);
		if(RefCnt == 0)
		{
			DeleteThis();
		}
		VASSERT(RefCnt >= 0);
		return (UINT)RefCnt;
	}
	inline UINT GetNumRef()	const	{ return (UINT)m_RefCounter; }
protected:
	virtual void DeleteThis()  = 0;
	mutable volatile int m_RefCounter;
};

template <class T> 
class VSmartPtr 
{
private:
	T* m_pObj;
public:
	VSmartPtr() : m_pObj(NULL) {}
	inline VSmartPtr(T* p_)
	{
		m_pObj = p_;
		if (m_pObj)
			m_pObj->AddRef();
	}

	inline VSmartPtr(const VSmartPtr &p_)
	{
		m_pObj = p_.m_pObj;
		if (m_pObj)
			m_pObj->AddRef();
	}
	inline ~VSmartPtr()
	{
		if (m_pObj)
			m_pObj->Release();
	}
	inline operator T*() const { return m_pObj; }
	inline T& operator*() const { return *m_pObj; }
	inline T* operator->(void) const { return m_pObj; }
	inline VSmartPtr&  operator=(T* newp)
	{
		if (newp)
			newp->AddRef();
		if (m_pObj)
			m_pObj->Release();
		m_pObj = newp;
		return *this;
	}
	inline VSmartPtr&  operator=(const VSmartPtr &newp)
	{
		if (newp.m_pObj)
			newp.m_pObj->AddRef();
		if (m_pObj)
			m_pObj->Release();
		m_pObj = newp.m_pObj;
		return *this;
	}

	inline bool  operator ==(T* p2) const 
	{
		return m_pObj == p2;
	};
	inline bool  operator ==(const VSmartPtr &p2) const 
	{
		return m_pObj == p2.m_pObj;
	};
	inline bool  operator !=(T* p2) const 
	{
		return m_pObj != p2;
	};
	inline bool  operator !=(const VSmartPtr &p2) const 
	{
		return m_pObj != p2.m_pObj;
	};
	/*inline bool  operator <(const T* p2) const 
	{
		return m_pObj < p2;
	};
	inline bool  operator >(const T* p2) const 
	{
		return m_pObj > p2;
	};*/
};

#endif 
