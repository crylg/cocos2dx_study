#pragma once

class VArchive;


class VSerializable
{
public:



private:
	VString		mName;
	VArchive*	mArchive;
};


class VArchive
{
public:

};



class VSerialize
{
public:
	VSerialize(void);
	virtual ~VSerialize(void);

	void BeginObject();
	void EndObject();


};

