#include "VStdAfx.h"
#include "KAsyncIO.h"

class KAsyncIOThread : public VThread
{
	virtual UINT Process()
	{
		return 0;
	}
	
};


KAsyncIO::KAsyncIO(void)
{
}


KAsyncIO::~KAsyncIO(void)
{
}
