#ifndef __CSVPASER_H__
#define __CSVPASER_H__

// CSV解析
class VCoreApi VCSVParser
{
public:
	VCSVParser();
	VCSVParser(void* pData, UINT Size, UINT LineMaxSize = 4096);
	~VCSVParser();

	// 解析CSV缓存
	void Parser(void* pData, UINT Size, UINT LineMaxSize = 4096);

	// 获得数据行数
	inline UINT GetRowNum()	const		{ return mData.size(); }

	// 通过行和列索引获得数据
	const char* GetField(UINT RowIndex, UINT ColIndex) const;

	// 通过行和列名获得数据
	const char* GetField(UINT RowIndex, const std::string& ColName) const;

	// 清理数据
	void Clear();

private:
	std::map<std::string, int>	mHeadMap;				// CSV第一行索引
	std::vector<std::vector<std::string*>*>	mData;		// 解析的数据
};

#endif // __CSVPASER_H__