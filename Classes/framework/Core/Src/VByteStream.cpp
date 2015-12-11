#include "VStdAfx.h"
#include "VByteStream.h"

void VByteStream::Resize(size_t newSize)
{
	VASSERT(!mRefBuffer);
	newSize = VAlign(newSize, 64);
	if (newSize > mCapacity)
	{
		UINT Grow = mCapacity >> 3;
		Grow = VAlign(Grow, 64);
		Grow = VClamp<UINT>(Grow, 64, 256);
		mCapacity = newSize + Grow;
		
		BYTE* Prev = mBuffer;
		mBuffer = (BYTE*)VMalloc(mCapacity);
		if (Prev)
		{
			if (mWritePos)
			{
				memcpy(mBuffer, Prev, mWritePos);
			}
			VFree(Prev);
		}
		
	}
}
