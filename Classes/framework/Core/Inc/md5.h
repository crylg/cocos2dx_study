#ifndef MD5_H
#define MD5_H
#include <string>

typedef unsigned char byte;
typedef unsigned long ulong;

class VCoreApi MD5
{
public:
	MD5();
	~MD5();
	void Begin();
	void Update(const void* data, ulong length);
	void Final();
	inline const byte* GetDigest() const	{ return _digest; }
	void ToString(char (&output)[33]) const;
private:
	ulong _state[4];	/* state (ABCD) */
	ulong _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	byte _buffer[64];	/* input buffer */
	byte _digest[16];	/* message digest */
};


// MD5¼ÓÃÜ ·µ»Ø×Ö·û´®
extern VCoreApi std::string MD5Encrypt(const void* data, ulong length);




#endif/*MD5_H*/