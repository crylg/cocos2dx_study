#ifndef __CSVPASER_H__
#define __CSVPASER_H__

// CSV����
class VCoreApi VCSVParser
{
public:
	VCSVParser();
	VCSVParser(void* pData, UINT Size, UINT LineMaxSize = 4096);
	~VCSVParser();

	// ����CSV����
	void Parser(void* pData, UINT Size, UINT LineMaxSize = 4096);

	// �����������
	inline UINT GetRowNum()	const		{ return mData.size(); }

	// ͨ���к��������������
	const char* GetField(UINT RowIndex, UINT ColIndex) const;

	// ͨ���к������������
	const char* GetField(UINT RowIndex, const std::string& ColName) const;

	// ��������
	void Clear();

private:
	std::map<std::string, int>	mHeadMap;				// CSV��һ������
	std::vector<std::vector<std::string*>*>	mData;		// ����������
};

#endif // __CSVPASER_H__