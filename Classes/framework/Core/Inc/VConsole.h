#ifndef __VCONSOLE_H__
#define __VCONSOLE_H__


struct VConsoleOutput
{

};

// ����̨����
struct VConsoleVariable
{

};


struct VConsoleVariableGroup : public VConsoleVariable
{

};

class VConsoleVarialbeInt : public VConsoleVariable
{
public:
	inline int Value() const { return m_Value; }
protected:
	int m_Value;
};

class VConsole
{
public:
	VConsole(void);
	~VConsole(void);
};




#endif 
