#include "VStdAfx.h"
#include "VEncrypt.h"
#include "sha1.h"

VSha1Hash::VSha1Hash()
{
	mSh1Context = VNew SHA1();
}

VSha1Hash::~VSha1Hash()
{
	VDelete ((SHA1*)mSh1Context);
}

void VSha1Hash::Initialize()
{
	((SHA1*)mSh1Context)->Reset();
}

void VSha1Hash::Finalize()
{
	((SHA1*)mSh1Context)->Result((unsigned int*)mDigest);
}

void VSha1Hash::UpdateData(const BYTE *data, int len)
{
	((SHA1*)mSh1Context)->Input(data, (unsigned int)len);
}