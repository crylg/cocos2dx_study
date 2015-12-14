#include "VStdAfx.h"
#include "VImageHelper.h"



#ifndef MIN
#define MIN(a, b)						((a) < (b) ? (a) : (b))
#endif

#define BE2LE32(v)	(((v & 0xFF000000) >> 24) & 0xFF | ((v & 0x00FF0000) >> 8) | ((v & 0x0000FF00) << 8) | ((v & 0x000000FF) << 24))
#define BE2LE16(v)	(((v & 0xFF00) >> 8) & 0xFF | ((v & 0xFF) << 8))

static const char * JPEG_SOI_TAG = "\xFF\xD8";
static const char * JPEG_EOI_TAG = "\xFF\xD9";
static const char * PNG_HEADER = "\x89PNG\r\n\x1A\n";

#pragma pack(push, 1)

struct stPNGIHDR {
	UINT	dwSize;
	char	szType[4];
	UINT	nWidth;
	UINT	nHeight;
	BYTE	nBitDepth;
	BYTE	nColorType;
	BYTE	nCompressionMethod;
	BYTE	nFilterMethod;
	BYTE	nInterllaceMethod;
};

struct stJPEGMarker {
	BYTE magic;
	BYTE type;
	unsigned short	wSize;
};

struct stJPEGSOF0 {
	BYTE	nBitsPerSample;
	unsigned short	nHeight;
	unsigned short	nWidth;
	BYTE	nColorComponents;
};

struct stJPEGSOFColorComponentInfo {
	BYTE	nComponentId;
	BYTE	nSamplingFactor;
	BYTE	nQuantizationTableNumber;
};

#pragma pack(pop)

bool VImageHelper::IsJPGImage(const std::string& path)
{
	bool bValid = false;
	FILE * fpJPEG = fopen(path.c_str(), "rb");
	if (fpJPEG)
	{
		bValid = ValidateJPGHeader(fpJPEG);
		fclose(fpJPEG);
	}
	return bValid;
}

bool VImageHelper::IsPNGImage(const std::string& path)
{
	bool bValid = false;
	FILE * fpPNG = fopen(path.c_str(), "rb");
	if (fpPNG)
	{
		bValid = ValidatePNGHeader(fpPNG);

		fclose(fpPNG);
	}
	return bValid;
}

bool VImageHelper::GetImageDimension(const std::string& path, int& width, int& height)
{
	FILE * fp = fopen(path.c_str(), "rb");
	bool bSuccess = false;
	if (fp)
	{
		if (ValidatePNGHeader(fp))
		{
			bSuccess = GetPNGGDimension(fp, width, height);
		}
		else if (ValidateJPGHeader(fp))
		{
			bSuccess = GetJPGDimension(fp, width, height);
		}

		fclose(fp);
	}
	return bSuccess;
}

bool VImageHelper::ValidateJPGHeader(FILE * fpJPEG)
{
	char szMarker1[3] = { 0 };
	char szMarker2[3] = { 0 };
	long int pos = ftell(fpJPEG);
	fseek(fpJPEG, 0, SEEK_SET);
	fread(szMarker1, 1, 2, fpJPEG);
	fseek(fpJPEG, -2, SEEK_END);
	fread(szMarker2, 1, 2, fpJPEG);
	fseek(fpJPEG, pos, SEEK_SET);
	if (!strcmp(JPEG_SOI_TAG, szMarker1) && !strcmp(JPEG_EOI_TAG, szMarker2))
	{
		return true;
	}
	return false;
}

bool VImageHelper::ValidateJPGHeader(void * pData, size_t nDataSize)
{
	BYTE * ptr = (BYTE *)pData;
	return !memcmp(ptr, JPEG_SOI_TAG, strlen(JPEG_SOI_TAG)) && !memcmp(ptr + nDataSize - 2, JPEG_EOI_TAG, strlen(JPEG_EOI_TAG));
}

bool VImageHelper::IsPNGImage(void * pData, size_t nDataSize)
{
	return ValidatePNGHeader(pData, nDataSize);
}
bool VImageHelper::IsJPGImage(void * pData, size_t nDataSize)
{
	return ValidateJPGHeader(pData, nDataSize);
}

bool VImageHelper::ValidatePNGHeader(FILE * fpPNG)
{
	char szHeader[9] = { 0 };
	long int pos = ftell(fpPNG);
	fseek(fpPNG, 0, SEEK_SET);
	fread(szHeader, 1, strlen(PNG_HEADER), fpPNG);
	fseek(fpPNG, pos, SEEK_SET);
	if (!strcmp(PNG_HEADER, szHeader))
	{
		return true;
	}
	return false;
}

bool VImageHelper::ValidatePNGHeader(void * pData, size_t nDataSize)
{
	return !memcmp(pData, PNG_HEADER, MIN(strlen(PNG_HEADER), nDataSize));
}

bool VImageHelper::GetPNGGDimension(FILE * fp, int& width, int& height)
{
	const int pngIHDROffset = 0x08;
	fseek(fp, pngIHDROffset, SEEK_SET);
	stPNGIHDR ihdr = { 0 };
	fread(&ihdr, sizeof(stPNGIHDR), 1, fp);
	// Check chunk type
	if (strncmp(ihdr.szType, "IHDR", sizeof(ihdr.szType)))
	{
		return false;
	}
	// Big-endian to little-endian
	ihdr.dwSize = BE2LE32(ihdr.dwSize);
	ihdr.nWidth = BE2LE32(ihdr.nWidth);
	ihdr.nHeight = BE2LE32(ihdr.nHeight);

	width = ihdr.nWidth;
	height = ihdr.nHeight;

	return true;
}

bool VImageHelper::GetPNGGDimension(void * pData, size_t nDataSize, int& width, int& height)
{
	const int pngIHDROffset = 0x08;
	stPNGIHDR ihdr = *(stPNGIHDR *)((BYTE *)pData + pngIHDROffset);
	// Check chunk type
	if (strncmp(ihdr.szType, "IHDR", sizeof(ihdr.szType)))
	{
		return false;
	}
	// Big-endian to little-endian
	ihdr.dwSize = BE2LE32(ihdr.dwSize);
	ihdr.nWidth = BE2LE32(ihdr.nWidth);
	ihdr.nHeight = BE2LE32(ihdr.nHeight);

	width = ihdr.nWidth;
	height = ihdr.nHeight;

	return true;
}

bool VImageHelper::GetJPGDimension(FILE * fp, int& width, int& height)
{
	bool bSuccess = false;
	fseek(fp, 2, SEEK_SET);
	while (!feof(fp))
	{
		stJPEGMarker marker = { 0 };
		fread(&marker, sizeof(stJPEGMarker), 1, fp);
		if (marker.magic != 0xFF)
		{
			// Bad marker.
			break;
		}
		marker.wSize = BE2LE16(marker.wSize);
		if (0xc0 <= marker.type && marker.type <= 0xcf && marker.type != 0xc4 && marker.type != 0xc8 && marker.type != 0xcc)
		{
			// SOFx segment
			stJPEGSOF0 sof = { 0 };
			fread(&sof, sizeof(stJPEGSOF0), 1, fp);
			sof.nWidth = BE2LE16(sof.nWidth);
			sof.nHeight = BE2LE16(sof.nHeight);
			width = sof.nWidth;
			height = sof.nHeight;
			bSuccess = true;
			break;
		}
		else
		{
			fseek(fp, marker.wSize - 2, SEEK_CUR);
		}

	}
	return bSuccess;
}

bool VImageHelper::GetJPGDimension(void * pData, size_t nDataSize, int& width, int& height)
{
	bool bSuccess = false;
	size_t offset = 2;
	BYTE * ptr = (BYTE *)pData + offset;
	while (offset < nDataSize)
	{
		stJPEGMarker marker = *(stJPEGMarker *)ptr;
		if (marker.magic != 0xFF)
		{
			// Bad marker.
			break;
		}
		marker.wSize = BE2LE16(marker.wSize);
		ptr += sizeof(stJPEGMarker);
		offset += sizeof(stJPEGMarker);
		if (0xc0 <= marker.type && marker.type <= 0xcf && marker.type != 0xc4 && marker.type != 0xc8 && marker.type != 0xcc)
		{
			// SOFx segment
			stJPEGSOF0 sof = *(stJPEGSOF0 *)ptr;
			sof.nWidth = BE2LE16(sof.nWidth);
			sof.nHeight = BE2LE16(sof.nHeight);
			width = sof.nWidth;
			height = sof.nHeight;
			bSuccess = true;
			break;
		}
		else
		{
			ptr += marker.wSize;
			offset += marker.wSize;
		}
	}
	return bSuccess;
}


#undef MIN