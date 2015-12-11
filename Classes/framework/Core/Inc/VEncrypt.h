#pragma once
#ifndef __VENCRYPT_H__
#define __VENCRYPT_H__

// SHA1 Hash. 
class VCoreApi VSha1Hash
{
public:
	enum
	{
		SHA_DIGEST_LENGTH = 20
	};

	VSha1Hash();
	~VSha1Hash();
	void UpdateData(const BYTE *dta, int len);
	void Initialize();
	void Finalize();

	inline const BYTE *GetDigest() const	{ return mDigest; };
	int GetLength(void) const		{ return SHA_DIGEST_LENGTH; };
private:
	void* mSh1Context;
	BYTE mDigest[SHA_DIGEST_LENGTH];
};

#endif 