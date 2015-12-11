#pragma once

class VCoreApi VComponent :  public VAllocator
{
public:
	VComponent();
	virtual ~VComponent();

	virtual BOOL GetOpenStatus(){ return m_open; };
	virtual void SetOpenStatus(bool b){m_open = b;};
	virtual BOOL IsTickable()const { return FALSE; }
protected:
	virtual void OnActive();
	virtual void OnDeactive();
	virtual void OnUpdate();
	
	bool m_open;
	
};


/**
 * 声明一个单件对象.
 */
#define VDECL_INSTANCE(Class) \
	private:	\
	static Class* _instance;\
	public:\
	static bool StaticInitInstance(); \
	static void StaticShutdownInstance(); \
	static inline Class* InstancePtr() { return _instance;}\
	static inline Class& Instance() { return *_instance; }


#define VIMPL_INSTANCE(Class) \
	Class* Class::_instance(nullptr);\
	bool Class::StaticInitInstance(){ \
		VASSERT(_instance==nullptr);\
		_instance = VNew Class;\
		return _instance->Init();\
	}\
	void Class::StaticShutdownInstance()\
	{\
		if(_instance){VDelete _instance; _instance = nullptr;}\
	}\


#define VINSTANCE_INIT(Class) Class::StaticInitInstance()
 
#define VINSTANCE_SHUTDOWN(Class) Class::StaticShutdownInstance()

#define VINSTANCE(Class) Class::Instance()