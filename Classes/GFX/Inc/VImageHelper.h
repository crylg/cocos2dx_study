#pragma once


class VCoreApi VImageHelper
{
public:
	static bool GetImageDimension(const std::string& path, int& width, int& height);
	static bool IsPNGImage(const std::string& path);
	static bool IsJPGImage(const std::string& path);

	static bool GetImageDimension(void * pData, size_t nDataSize, int& width, int& height);
	static bool IsPNGImage(void * pData, size_t nDataSize);
	static bool IsJPGImage(void * pData, size_t nDataSize);

private:
	static bool ValidatePNGHeader(FILE * fp);
	static bool ValidateJPGHeader(FILE * fp);
	static bool GetPNGGDimension(FILE * fp, int& width, int& height);
	static bool GetJPGDimension(FILE * fp, int& width, int& height);
	static bool ValidatePNGHeader(void * pData, size_t nDataSize);
	static bool ValidateJPGHeader(void * pData, size_t nDataSize);
	static bool GetPNGGDimension(void * pData, size_t nDataSize, int& width, int& height);
	static bool GetJPGDimension(void * pData, size_t nDataSize, int& width, int& height);
};

