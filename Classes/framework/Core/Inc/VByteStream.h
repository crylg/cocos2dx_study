#pragma once
#ifndef __VBYTESTREAM_H__
#define __VBYTESTREAM_H__

class VCoreApi VByteStream
{
public:
	const static size_t DEFAULT_SIZE = 128;

	inline VByteStream() : mBuffer(NULL), mCapacity(0), mReadPos(0), mWritePos(0), mRefBuffer(FALSE)
	{
		Resize(DEFAULT_SIZE);
	}
	inline VByteStream(size_t res) : mBuffer(NULL), mCapacity(0), mReadPos(0), mWritePos(0), mRefBuffer(FALSE)
	{
		Resize(res);
	}

	inline VByteStream(void* buf, size_t size) : mBuffer((BYTE*)buf), mCapacity(size), mReadPos(0), mWritePos(size), mRefBuffer(TRUE)
	{

	}

	virtual ~VByteStream()
	{
		if (mBuffer && !mRefBuffer)
		{
			VFree(mBuffer);
		}
	}

	void Clear()
	{
		if (!mRefBuffer)
		{
			if (mBuffer)
			{
				VFree(mBuffer); mBuffer = NULL;
			}
			mCapacity = 0;
		}
		mReadPos = mWritePos = 0;
	}

	inline void Clear_NotFree()
	{
		mReadPos = mWritePos = 0;
	}

	inline void InitMemoryStream(void* Memory, size_t size)
	{

	}

	template <typename T>
	void Write(T value)
	{
		Write((BYTE*)&value, sizeof(value));
	}
	template <typename T>
	void put(size_t pos, T value)
	{
		put(pos, (BYTE *)&value, sizeof(value));
	}

	// stream like operators for storing data
	inline VByteStream& operator << (bool value)
	{
		Write<char>((char)value);
		return *this;
	}
	// unsigned
	inline VByteStream &operator<<(BYTE value) {
		Write<BYTE>(value);
		return *this;
	}
	inline VByteStream &operator<<(WORD value) {
		Write<WORD>(value);
		return *this;
	}

	inline VByteStream &operator<<(UINT value) {
		Write<UINT>(value);
		return *this;
	}
	inline VByteStream &operator<<(UINT64 value) {

		Write<UINT64>(value);

		return *this;
	}
	inline VByteStream &operator<<(char value) {
		Write<char>(value);
		return *this;
	}
	inline VByteStream &operator<<(SHORT value) {

		Write<SHORT>(value);

		return *this;
	}
	inline VByteStream &operator<<(int value) {

		Write<int>(value);

		return *this;
	}
	inline VByteStream &operator<<(INT64 value) {

		Write<INT64>(value);

		return *this;
	}
	inline VByteStream &operator<<(float value)
	{
		Write<float>(value);
		return *this;
	}

	inline VByteStream &operator<<(double value)
	{
		Write<double>(value);
		return *this;
	}

	inline VByteStream &operator<<(const std::string &value)
	{
		Write((UINT)value.length());
		Write((BYTE *)value.c_str(), value.length());
		return *this;
	}
	inline VByteStream &operator<<(const char *str)
	{
		UINT len = strlen(str);
		Write(len);
		Write((BYTE *)str, strlen(str));
		return *this;
	}

	// stream like operators for reading data
	inline VByteStream &operator>>(bool &value) {
		value = Read<char>() > 0 ? true : false;
		return *this;
	}
	//unsigned
	inline VByteStream &operator>>(BYTE &value) {
		value = Read<BYTE>();
		return *this;
	}
	inline VByteStream &operator>>(WORD &value) {

		value = Read<WORD>();

		return *this;
	}
	inline VByteStream &operator>>(UINT &value) {

		value = Read<UINT>();
		return *this;
	}
	inline VByteStream &operator>>(UINT64 &value) {

		value = Read<UINT64>();

		return *this;
	}
	//signed as in 2e complement
	inline VByteStream &operator>>(char &value) {
		value = Read<char>();
		return *this;
	}
	inline VByteStream &operator>>(SHORT &value) {
		value = Read<SHORT>();
		return *this;
	}
	inline VByteStream &operator>>(int &value) {
		value = Read<int>();
		return *this;
	}
	inline VByteStream &operator>>(INT64 &value) {
		value = Read<INT64>();
		return *this;
	}
	inline VByteStream &operator>>(float &value) {
		value = Read<float>();
		return *this;
	}
	inline VByteStream &operator>>(double &value) {
		value = Read<double>();
		return *this;
	}
	inline VByteStream &operator>>(std::string& value)
	{
		value.clear();
		UINT len = Read<UINT>();
		if (len)
		{
			value.resize(len, 0);
			Read(&value[0], len);
		}
		return *this;
	}

	inline BYTE operator[](size_t pos) {
		return Read<BYTE>(pos);
	}

	inline size_t rpos() {
		return mReadPos;
	};

	inline size_t rpos(size_t rpos) {
		mReadPos = rpos;
		return mReadPos;
	};

	inline size_t wpos() {
		return mWritePos;
	}

	inline size_t wpos(size_t wpos) {
		mWritePos = wpos;
		return mWritePos;
	}

	template <typename T> 
	inline T Read() 
	{
		T r = Read<T>(mReadPos);
		mReadPos += sizeof(T);
		return r;
	}

	template <typename T>
	inline T Read(size_t pos) const
	{
		if (pos + sizeof(T) > size())
		{
			return (T)0;
		}
		else
		{
			return *((T*)&mBuffer[pos]);
		}
	}

	inline void Read(void *dest, size_t len)
	{
		VASSERT(mReadPos + len <= size());
		if (mReadPos + len > size())
		{
			return;
		}
		memcpy(dest, &mBuffer[mReadPos], len);
		mReadPos += len;
	}

	int ReadCString(char* Dest, int DesMaxSizeInBytes)
	{
		int index = 0;
		char c;
		while (index < DesMaxSizeInBytes)
		{
			c = Read<char>();
			Dest[index] = c;
			if (c == 0)
			{
				return index;
			}
			index++;
		}
		return index;
	}

	inline const BYTE *contents() const { return &mBuffer[0]; };

	inline size_t size() const { return mWritePos; };

	inline void Write(const std::string& str)
	{
		Write((BYTE *)str.c_str(), str.size());
	}
	inline void Write(const char *src, size_t cnt)
	{
		Write(&cnt, sizeof(size_t));
		Write((const void*)src, cnt);
	}
	inline void Write(const void *src, size_t cnt)
	{
		if (!cnt)
			return;
		if (mCapacity < (mWritePos + cnt))
		{
			Resize(mWritePos + cnt);
		}
		memcpy(&mBuffer[mWritePos], src, cnt);
		mWritePos += cnt;
	}
	inline void WriteCString(const char* src, int len = 0)
	{
		if (len == 0)
		{
			len = strlen(src);
		}
		Write((const void*)src, len);
		Write((char)0);
	}
	inline void Write(const VByteStream& buffer)
	{
		if (buffer.size() > 0)
			Write(buffer.contents(), buffer.size());
	}

	inline void put(size_t pos, const BYTE *src, size_t cnt)
	{
		assert(pos + cnt <= size());
		memcpy(&mBuffer[pos], src, cnt);
	}

	template <typename T>
	void WriteVector(const std::vector<T>& Objects)
	{
		unsigned short size = Objects.size();
		Write(size);
		if (size > 0)
		{
			for (auto& temp : Objects)
			{
				*this << temp;
			}
		}
	}

	template <typename T>
	bool ReadVector(std::vector<T>& Objects)
	{
		unsigned short size = Read<unsigned short>();
		if (size > 0)
		{
			Objects.reserve(size);
			for (unsigned int i = 0; i < size; i++)
			{
				T t;
				*this >> t;
				Objects.push_back(t);
			}
		}
		return true;
	}

	virtual void Resize(size_t newSize);
protected:
	BYTE*	mBuffer;
	UINT	mCapacity;
	UINT	mReadPos;
	UINT	mWritePos;
	UINT	mRefBuffer;
};




template<int StackSize = 256>
class VStackByteStream : public VByteStream
{
public:
	inline VStackByteStream()
	{
		mBuffer = mStackData;
		mCapacity = (UINT)StackSize;
	}

	virtual ~VStackByteStream()
	{
		if (mCapacity == (UINT)StackSize)
		{
			mBuffer = NULL;
		}
	}


	void Resize(size_t newSize)
	{
		if (newSize > StackSize)
		{
			if (mCapacity == (UINT)StackSize)
			{
				mBuffer = NULL;
				VByteStream::Resize(newSize);
				if (mWritePos)
				{
					memcpy(mBuffer, mStackData, mWritePos);
				}
			}
			else
			{
				VByteStream::Resize(newSize);
			}
		}
	}
private:
	BYTE mStackData[StackSize];
};


#endif 
