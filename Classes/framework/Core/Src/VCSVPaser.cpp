#include "VStdAfx.h"
#include "VCSVPaser.h"

static char* GetLine(const char* pSrc, char*& pDest, UINT& uMaxLen, UINT& LeftSize)
{
	char* pc = (char*)pSrc;

	// 忽略上次的\r\n 
	while( LeftSize > 0 && (*pc=='\r' || *pc=='\n') )
	{
		LeftSize--;
		pc++;
	}

	if (LeftSize <= 0)
	{
		return nullptr;
	}

	char* pStart = pc;
	while( LeftSize > 0 && *pc!='\r' && *pc!='\n' )
	{
		LeftSize--;
		pc++;
	}

	char* pEnd = pc;
	UINT uCurLen = pEnd - pStart;
	if (uCurLen >= uMaxLen)
	{
		uMaxLen = uCurLen + 256 + 1;
		uMaxLen = (uMaxLen + 3) & ~3;
		pDest = (char*)realloc(pDest, uMaxLen);
	}
	for (UINT c = 0; c < uCurLen; c++)
	{
		pDest[c] = *(pStart + c);
	}
	pDest[uCurLen] = 0;
	return pc;
}

VCSVParser::VCSVParser()
{

}

VCSVParser::VCSVParser(void* pData, UINT Size, UINT LineMaxSize /* = 4096 */)
{
	Parser(pData,Size,LineMaxSize);
}

VCSVParser::~VCSVParser()
{
	Clear();
}

void VCSVParser::Parser(void* pData, UINT Size, UINT LineMaxSize /* = 4096 */)
{
	if (pData == nullptr)
	{
		return;
	}
	char* pChar = (char*)pData;
	UINT MaxLine = LineMaxSize;
	char* szBuf = (char*)malloc(MaxLine);		// max line.
	UINT RowBufLen = 0;
	UINT NumRows = 0;
	UINT NumCols = 0;
	UINT ColIndex = 0;
	const char* pszSeps = ",";
	char* pszToken = nullptr;
	UINT LeftSize = Size;
	std::vector<std::string*>* pRowData;
	while(true)
	{
		pChar = GetLine(pChar, szBuf, MaxLine, LeftSize);
		if (pChar == nullptr)
		{
			break;
		}
		if (NumRows == 0)
		{
			ColIndex = 0;
			pszToken = strtok(szBuf, pszSeps);
			while(pszToken)
			{
				mHeadMap.insert(std::map<std::string, int>::value_type(std::string(pszToken),ColIndex));
				ColIndex++;
				pszToken = strtok(NULL, pszSeps);
			}
			NumRows++;
			NumCols = ColIndex;
		}else
		{
			pRowData = new std::vector<std::string*>;
			ColIndex = 0;
			pszToken = strtok(szBuf, pszSeps);
			while(pszToken)
			{
				pRowData->push_back(new std::string(pszToken));
				ColIndex++;
				pszToken = strtok(NULL, pszSeps);
			}
			if (ColIndex != NumCols)
			{
				// 列数错误
				VLOG_ERROR("Column num[%d] error at row[%d] : correct col num is [%d].", ColIndex, NumRows+1, NumCols);
				assert(0);
				delete pRowData;
				pRowData = nullptr;
				break;
			}
			mData.push_back(pRowData);
			NumRows++;
		}
		if (LeftSize <= 0)
		{
			break;
		}
	}
	free(szBuf);
}

void VCSVParser::Clear()
{
	mHeadMap.clear();
	for (auto& temp : mData)
	{
		for (auto& str : *temp)
		{
			delete str;
		}
		delete temp;
	}
}

const char* VCSVParser::GetField(UINT RowIndex, UINT ColIndex) const
{
	if (RowIndex >= mData.size())
	{
		assert(0);
		VLOG_ERROR("row[%d] overflow: max[%d]",RowIndex,(int)mData.size());
		return "";
	}
	auto& temp = mData[RowIndex];
	if (ColIndex >= temp->size())
	{
		assert(0);
		VLOG_ERROR("col[%d] overflow: max[%d]",ColIndex,(int)temp->size());
		return "";
	}
	const std::string* ss = (*temp)[ColIndex];
	return ss->c_str();
}

const char* VCSVParser::GetField(UINT RowIndex, const std::string& ColName) const
{
	auto itr = mHeadMap.find(ColName);
	if (itr == mHeadMap.end())
	{
		return "";
	}
	int ColIndex = itr->second;
	return GetField(RowIndex,ColIndex);
}


