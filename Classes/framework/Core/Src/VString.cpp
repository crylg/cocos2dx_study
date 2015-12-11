#include "VStdAfx.h"
#include "VString.h"
#include "KCorePrivate.h"

/*
CHAR gc_LowerMap[256];
CHAR gc_UperMap[256];
void InitMap()
{
	for (int i = 0; i < 256; i++) 
	{
		gc_LowerMap[i] = tolower(i);
		gc_UperMap[i] = toupper(i);
	}
}

void PrintfArray(FILE* fp, CHAR* Arr, int Num)
{
	for (int I = 0; I < Num; I++)
	{
		if ((I % 16) == 0)
		{
			fprintf(fp, "\r\n\t");
		}
		fprintf(fp, "0x%02X", (BYTE)gc_LowerMap[I]);
		if (I != (Num - 1))
		{
			fprintf(fp, "%s", ",");
		}
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	InitMap();
	FILE* fp = fopen("c:/lower_uper_map.txt", "wt+");
	fprintf(fp, "CHAR gc_LowerMap[256] = {");
	PrintfArray(fp, gc_LowerMap, 256);
	fprintf(fp,"\r\n};");
	fprintf(fp, "CHAR gc_UperMap[256] = {");
	PrintfArray(fp, gc_UperMap, 256);
	fprintf(fp,"\r\n};");
	fclose(fp);
	return 0;
}
*/
const BYTE vc_LowerMap[256] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
	0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
	0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
	0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
	0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
	0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
	0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
const BYTE vc_UpperMap[256] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
	0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
	0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
	0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
	0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
	0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
	0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
#if 0

VString::VString(void)
{
}


VString::~VString(void)
{
}


/*
 */
const VString VString::null;

/*
 */
VString::VString() : m_Size(0), m_Capacity(CAPACITY), m_Str(stack_data) {
	m_Str[m_Size] = '\0';
}

VString::VString(const VString &s) : m_Size(0), m_Capacity(CAPACITY), m_Str(stack_data) {
	copy(s);
}

VString::VString(const char *s) : m_Size(0), m_Capacity(CAPACITY), m_Str(stack_data) {
	copy(s);
}

VString::~VString() {
	destroy();
}

/*
 */
VString &VString::operator=(const char *s) {
	if(m_Str == s) return *this;
	if(s == NULL) clear();
	else do_copy(s,(int)strlen(s));
	return *this;
}

VString &VString::operator=(const VString &s) {
	if(this == &s) return *this;
	do_copy(s,s.m_Size);
	return *this;
}

/*
 */
VString &VString::operator+=(char c) {
	do_append(m_Size,c);
	return *this;
}

VString &VString::operator+=(const char *s) {
	if(s == NULL) return *this;
	do_append(m_Size,s,(int)strlen(s));
	return *this;
}

VString &VString::operator+=(const VString &s) {
	do_append(m_Size,s.m_Str,s.m_Size);
	return *this;
}

/*
 */
void VString::resize(int size) {
	allocate(size);
	m_Size = size;
	m_Str[m_Size] = '\0';
}

void VString::allocate(int size) {
	if(size + 1 <= (m_Capacity & CAPACITY_MASK)) return;
	int dynamic = (m_Capacity & DYNAMIC_FLAG);
	m_Capacity = (size + 1) | DYNAMIC_FLAG;
	char *new_data = new char[size + 1];
	do_memcpy(new_data,m_Str,m_Size);
	if(dynamic) delete [] m_Str;
	m_Str = new_data;
	m_Str[m_Size] = '\0';
}

void VString::reserve(int size) {
	if(size + 1 <= (m_Capacity & CAPACITY_MASK)) return;
	int dynamic = (m_Capacity & DYNAMIC_FLAG);
	m_Capacity = (size * 2 + 1) | DYNAMIC_FLAG;
	char *new_data = new char[size * 2 + 1];
	do_memcpy(new_data,m_Str,m_Size);
	if(dynamic) delete [] m_Str;
	m_Str = new_data;
	m_Str[m_Size] = '\0';
}

/*
 */
void VString::clear() {
	m_Size = 0;
	m_Str[m_Size] = '\0';
}

void VString::destroy() {
	int dynamic = (m_Capacity & DYNAMIC_FLAG);
	m_Size = 0;
	m_Capacity = CAPACITY;
	if(dynamic) delete [] m_Str;
	m_Str = stack_data;
	m_Str[m_Size] = '\0';
}

/*
 */
int VString::find(char c) const {
	for(int i = 0; i < m_Size; i++) if(m_Str[i] == c) return i;
	return -1;
}

int VString::find(const char *s) const {
	if(s == NULL) return -1;
	int len = (int)strlen(s);
	for(int i = 0; i < m_Size; i++) if(!strncmp(m_Str + i,s,len)) return i;
	return -1;
}

int VString::find(const VString &s) const {
	for(int i = 0; i < m_Size; i++) if(!strncmp(m_Str + i,s.m_Str,s.m_Size)) return i;
	return -1;
}

/*
 */
int VString::rfind(char c) const {
	for(int i = m_Size - 1; i >= 0; i--) if(m_Str[i] == c) return i;
	return -1;
}

int VString::rfind(const char *s) const {
	if(s == NULL) return -1;
	int len = (int)strlen(s);
	for(int i = m_Size - 1; i >= 0; i--) if(!strncmp(m_Str + i,s,len)) return i;
	return -1;
}

int VString::rfind(const VString &s) const {
	for(int i = m_Size - 1; i >= 0; i--) if(!strncmp(m_Str + i,s.m_Str,s.m_Size)) return i;
	return -1;
}

/*
 */
void VString::copy(const char *s,int len) {
	if(s == NULL) {
		clear();
	} else {
		if(len == -1) len = (int)strlen(s);
		do_copy(s,len);
	}
}

void VString::copy(const VString &s,int len) {
	if(len == -1) len = s.m_Size;
	do_copy(s.m_Str,len);
}

/*
 */
void VString::append(char c) {
	do_append(m_Size,c);
}

void VString::append(const char *s,int len) {
	if(s == NULL) return;
	if(len == -1) len = (int)strlen(s);
	do_append(m_Size,s,len);
}

void VString::append(const VString &s,int len) {
	if(len == -1) len = s.m_Size;
	do_append(m_Size,s.m_Str,len);
}

void VString::append(int pos,char c) {
	do_append(pos,c);
}

void VString::append(int pos,const char *s,int len) {
	if(s == NULL) return;
	if(len == -1) len = (int)strlen(s);
	do_append(pos,s,len);
}

void VString::append(int pos,const VString &s,int len) {
	if(len == -1) len = s.m_Size;
	do_append(pos,s.m_Str,len);
}

/*
 */
void VString::remove() {
	assert(m_Size > 0 && "VString::remove(): bad m_Size");
	m_Str[--m_Size] = '\0';
}

void VString::remove(int pos,int len) {
	assert(pos >= 0 && pos <= m_Size && "VString::remove(): bad position");
	assert(len >= 0 && pos + len <= m_Size && "VString::remove(): bad m_Size");
	do_memcpy(m_Str + pos,m_Str + pos + len,m_Size - pos - len);
	m_Size -= len;
	m_Str[m_Size] = '\0';
}

/*
 */
void VString::do_copy(const char *s,int len) {
	m_Size = len;
	if(m_Size < (m_Capacity & CAPACITY_MASK)) {
		do_memcpy(m_Str,s,m_Size);
	} else {
		int dynamic = (m_Capacity & DYNAMIC_FLAG);
		m_Capacity = (m_Size + 1) | DYNAMIC_FLAG;
		char *new_data = new char[m_Size + 1];
		do_memcpy(new_data,s,m_Size);
		if(dynamic) delete [] m_Str;
		m_Str = new_data;
	}
	m_Str[m_Size] = '\0';
}

void VString::do_append(int pos,char c) {
	assert(pos >= 0 && pos <= m_Size && "VString::do_append(): bad position");
	int new_length = m_Size + 1;
	if(new_length < (m_Capacity & CAPACITY_MASK)) {
		do_memcpy(m_Str + pos + 1,m_Str + pos,m_Size - pos);
		m_Str[pos] = c;
	} else {
		int dynamic = (m_Capacity & DYNAMIC_FLAG);
		m_Capacity = (new_length * 2 + 1) | DYNAMIC_FLAG;
		char *new_data = new char[new_length * 2 + 1];
		do_memcpy(new_data,m_Str,pos);
		new_data[pos] = c;
		do_memcpy(new_data + pos + 1,m_Str + pos,m_Size - pos);
		if(dynamic) delete [] m_Str;
		m_Str = new_data;
	}
	m_Size = new_length;
	m_Str[m_Size] = '\0';
}

void VString::do_append(int pos,const char *s,int len) {
	assert(pos >= 0 && pos <= m_Size && "VString::do_append(): bad position");
	int new_length = m_Size + len;
	if(new_length < (m_Capacity & CAPACITY_MASK)) {
		do_memcpy(m_Str + pos + len,m_Str + pos,m_Size - pos);
		do_memcpy(m_Str + pos,s,len);
	} else {
		int dynamic = (m_Capacity & DYNAMIC_FLAG);
		m_Capacity = (new_length * 2 + 1) | DYNAMIC_FLAG;
		char *new_data = new char[new_length * 2 + 1];
		do_memcpy(new_data,m_Str,pos);
		do_memcpy(new_data + pos,s,len);
		do_memcpy(new_data + pos + len,m_Str + pos,m_Size - pos);
		if(dynamic) delete [] m_Str;
		m_Str = new_data;
	}
	m_Size = new_length;
	m_Str[m_Size] = '\0';
}

void VString::do_memcpy(char *dest,const char *src,int size) {
	if(dest > src && (int)(dest - src) < size) {
		dest += size - 1;
		src += size - 1;
		for(int i = size; i > 0; i--) {
			*dest-- = *src--;
		}
	} else {
		#ifdef _WEBGL
			memcpy(dest,src,size);
		#else
			if(size & ~15) {
				for(size_t i = size >> 4; i > 0; i--) {
					*(unsigned int*)(dest + 0) = *(const unsigned int*)(src + 0);
					*(unsigned int*)(dest + 4) = *(const unsigned int*)(src + 4);
					*(unsigned int*)(dest + 8) = *(const unsigned int*)(src + 8);
					*(unsigned int*)(dest + 12) = *(const unsigned int*)(src + 12);
					dest += 16;
					src += 16;
				}
				size &= 15;
			}
			if(size & ~3) {
				for(size_t i = size >> 2; i > 0; i--) {
					*(unsigned int*)dest = *(const unsigned int*)src;
					dest += 4;
					src += 4;
				}
				size &= 3;
			}
			if(size) {
				for(int i = size; i > 0; i--) {
					*dest++ = *src++;
				}
			}
		#endif
	}
}

/*
 */
void VString::lower() {
	char *s = m_Str;
	while(*s) {
		char c = *s;
		if(c >= 'A' && c <= 'Z') *s += ('a' - 'A');
		s++;
	}
}

void VString::upper() {
	char *s = m_Str;
	while(*s) {
		char c = *s;
		if(c >= 'a' && c <= 'z') *s -= ('a' - 'A');
		s++;
	}
}

/*
 */
void VString::vprintf(const char *format,va_list argptr) 
{
	clear();
	char buf[1024];
	StringStack<> token;
	const char *f = format;
	while(*f) {
		if(*f == '%') {
			token.clear();
			token.append(*f++);
			int is_long = 0;
			while(*f && strchr("0123456789.-+#lL ",*f)) {
				if(*f == 'l' || *f == 'L') is_long = 1;
				token.append(*f++);
			}
			token.append(*f);
			buf[0] = '\0';
			if(strchr("cdiouxX",*f)) {
				if(is_long) sprintf(buf,token,va_arg(argptr,long long));
				else sprintf(buf,token,va_arg(argptr,int));
			} else if(strchr("fgGeE",*f)) {
				sprintf(buf,token,va_arg(argptr,double));
			} else if(strchr("s",*f)) {
				append(va_arg(argptr,const char*));
			} else if(strchr("p",*f)) {
				sprintf(buf,token,va_arg(argptr,const void*));
			} else if(strchr("%",*f)) {
				strcpy(buf,"%");
			} else {
				break;
			}
			append(buf);
			if(*f) f++;
		} else {
			append(*f++);
		}
	}
}

void VString::printf(const char *format,...) {
	va_list argptr;
	va_start(argptr,format);
	vprintf(format,argptr);
	va_end(argptr);
}

/*
 */
int VString::vscanf(const char *format,va_list argptr) {
	int ret = 0;
	StringStack<> token;
	const char *s = m_Str;
	const char *f = format;
	const char *delimiters = " \t\n\r";
	while(*s && *f) {
		while(*f && strchr(delimiters,*f)) f++;
		if(*f == '%') {
			while(*s && strchr(delimiters,*s)) s++;
			if(*s == '\0') break;
			f++;
			token.clear();
			int is_long = 0;
			while(*f && strchr("0123456789.lL ",*f)) {
				if(*f == 'l' || *f == 'L') is_long = 1;
				token.append(*f++);
			}
			int m_Size = atoi(token);
			token.clear();
			if(strchr("dDiI",*f)) {
				if(m_Size == 0) while(*s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				else while(m_Size-- && *s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				if(is_long) *((long long*)va_arg(argptr,long long*)) = atoi(token);
				else *((int*)va_arg(argptr,int*)) = atoi(token);
				ret++;
			} else if(strchr("xX",*f)) {
				if(m_Size == 0) while(*s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				else while(m_Size-- && *s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				if(is_long) *((long long*)va_arg(argptr,long long*)) = atol("0x" + token);
				else *((int*)va_arg(argptr,int*)) = atoi("0x" + token);
				ret++;
			} else if(strchr("fFgGeE",*f)) {
				if(m_Size == 0) while(*s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				else while(m_Size-- && *s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				if(is_long) *((double*)va_arg(argptr,double*)) = atod(token);
				else *((float*)va_arg(argptr,float*)) = atof(token);
				ret++;
			} else if(strchr("sS",*f)) {
				char *d = (char*)va_arg(argptr,char*);
				if(m_Size == 0) while(*s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				else while(m_Size-- && *s && strchr(delimiters,*s) == NULL && *s != *(f + 1)) token.append(*s++);
				*d = '\0';
				ret++;
			} else if(strchr("cC",*f)) {
				*((char*)va_arg(argptr,char*)) = *s++;
				ret++;
			} else {
				break;
			}
			f++;
		} else {
			while(*s && strchr(delimiters,*s)) s++;
			while(*f && *s && *f == *s) {
				if(strchr(delimiters,*f)) break;
				f++;
				s++;
			}
			if(*f != '%' && *f != *s) break;
		}
	}
	return ret;
}

int VString::scanf(const char *format,...) 
{
	va_list argptr;
	va_start(argptr,format);
	int ret = vscanf(format,argptr);
	va_end(argptr);
	return ret;
}

/******************************************************************************\
*
* VString functions
*
\******************************************************************************/

/*
 */
StringStack<> operator+(const VString &s0,const VString &s1) {
	StringStack<> ret = s0;
	ret.append(s1);
	return ret;
}

StringStack<> operator+(const VString &s0,const char *s1) {
	StringStack<> ret = s0;
	ret.append(s1);
	return ret;
}

StringStack<> operator+(const char *s0,const VString &s1) {
	StringStack<> ret = StringStack<>(s0);
	ret.append(s1);
	return ret;
}

StringStack<> operator+(const VString &s0,char c1) {
	StringStack<> ret = s0;
	ret.append(c1);
	return ret;
}

StringStack<> operator+(char c0,const VString &s1) {
	StringStack<> ret;
	ret.append(c0);
	ret.append(s1);
	return ret;
}

/*
 */
#define DECLARE_STRING_COMPARE(OP) \
int operator OP(const VString &s0,const VString &s1) { \
	return (strcmp(s0.get(),s1.get()) OP 0); \
} \
int operator OP(const VString &s0,const char *s1) { \
	if(s1 == NULL) return 0; \
	return (strcmp(s0.get(),s1) OP 0); \
} \
int operator OP(const char *s0,const VString &s1) { \
	if(s0 == NULL) return 0; \
	return (strcmp(s0,s1.get()) OP 0); \
}

DECLARE_STRING_COMPARE(==)
DECLARE_STRING_COMPARE(!=)
DECLARE_STRING_COMPARE(<)
DECLARE_STRING_COMPARE(>)
DECLARE_STRING_COMPARE(<=)
DECLARE_STRING_COMPARE(>=)

#undef DECLARE_STRING_COMPARE

/******************************************************************************\
*
* VString static functions
*
\******************************************************************************/

/*
 */
int VString::isdigit(int code) {
	return (code >= '0' && code <= '9');
}

int VString::islower(int code) {
	return (code >= 'a' && code <= 'z');
}

int VString::isupper(int code) {
	return (code >= 'A' && code <= 'Z');
}

int VString::isalpha(int code) {
	return ((code >= 'a' && code <= 'z') || (code >= 'A' && code <= 'Z'));
}

/*
 */
int VString::atoi(const char *str) {
	int ret = 0;
	int negative = 0;
	const char *s = str;
	while(*s == ' ' || *s == '\t') {
		s++;
	}
	if(*s == '-' || *s == '+') {
		if(*s == '-') negative = 1;
		s++;
	}
	if(*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')) {
		s += 2;
		while(*s) {
			char c = *s++;
			if(c >= '0' && c <= '9') ret = (ret << 4) + (c - '0');
			else if(c >= 'a' && c <= 'f') ret = (ret << 4) + (c - 'a' + 10);
			else if(c >= 'A' && c <= 'F') ret = (ret << 4) + (c - 'A' + 10);
			else break;
		}
	} else {
		while(*s) {
			char c = *s++;
			if(c < '0' || c > '9') break;
			ret = ret * 10 + (c - '0');
		}
	}
	if(negative) return -ret;
	return ret;
}

long long VString::atol(const char *str) {
	long long ret = 0;
	int negative = 0;
	const char *s = str;
	while(*s == ' ' || *s == '\t') {
		s++;
	}
	if(*s == '-' || *s == '+') {
		if(*s == '-') negative = 1;
		s++;
	}
	if(*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')) {
		s += 2;
		while(*s) {
			char c = *s++;
			if(c >= '0' && c <= '9') ret = (ret << 4) + (c - '0');
			else if(c >= 'a' && c <= 'f') ret = (ret << 4) + (c - 'a' + 10);
			else if(c >= 'A' && c <= 'F') ret = (ret << 4) + (c - 'A' + 10);
			else break;
		}
	} else {
		while(*s) {
			char c = *s++;
			if(c < '0' || c > '9') break;
			ret = ret * 10 + (c - '0');
		}
	}
	if(negative) return -ret;
	return ret;
}

float VString::atof(const char *str) {
	return (float)atod(str);
}

double VString::atod(const char *str) {
	double ret = 0.0;
	int negative = 0;
	const char *s = str;
	while(*s == ' ' || *s == '\t') {
		s++;
	}
	if(*s == '-' || *s == '+') {
		if(*s == '-') negative = 1;
		s++;
	}
	if(*s >= '0' && *s <= '9') {
		while(*s) {
			char c = *s;
			if(c < '0' || c > '9') break;
			ret = ret * 10.0 + (c - '0');
			s++;
		}
	}
	if(*s == '.') {
		s++;
		if(*s >= '0' && *s <= '9') {
			double frac = 0.0;
			double scale = 0.1;
			while(*s) {
				char c = *s;
				if(c < '0' || c > '9') break;
				frac += scale * (c - '0');
				scale *= 0.1;
				s++;
			}
			ret += frac;
		}
	}
	if(*s == 'e' || *s == 'E') {
		s++;
		double exp = 0.0;
		int negative = 0;
		if(*s == '-' || *s == '+') {
			if(*s == '-') negative = 1;
			s++;
		}
		if(*s >= '0' && *s <= '9') {
			while(*s) {
				char c = *s++;
				if(c < '0' || c > '9') break;
				exp = exp * 10.0 + (c - '0');
			}
		}
		if(negative) exp = -exp;
		ret *= pow(10.0,exp);
	}
	if(negative) return -ret;
	return ret;
}

/*
 */
StringStack<> VString::memory(size_t size) {
	StringStack<> ret;
	if(size < 1024) ret.printf("%dB",size);
	else if(size < 1024 * 1024) ret.printf("%.1fKB",size / 1024.0);
	else if(size < 1024 * 1024 * 1024) ret.printf("%.1fMB",size / (1024.0 * 1024.0));
	else ret.printf("%.1fGB",size / (1024.0 * 1024.0 * 1024.0));
	return ret;
}

/*
 */
StringStack<> VString::lower(const char *str) {
	StringStack<> ret = StringStack<>(str);
	ret.lower();
	return ret;
}

StringStack<> VString::upper(const char *str) {
	StringStack<> ret = StringStack<>(str);
	ret.upper();
	return ret;
}

/*
 */
StringStack<> VString::format(const char *format,va_list argptr) {
	StringStack<> ret;
	ret.vprintf(format,argptr);
	return ret;
}

StringStack<> VString::format(const char *format,...) {
	StringStack<> ret;
	va_list argptr;
	va_start(argptr,format);
	ret.vprintf(format,argptr);
	va_end(argptr);
	return ret;
}

/*
 */
int VString::vsscanf(const char *str,const char *format,va_list argptr) {
	return StringStack<>(str).vscanf(format,argptr);
}

int VString::sscanf(const char *str,const char *format,...) {
	va_list argptr;
	va_start(argptr,format);
	int ret = StringStack<>(str).vscanf(format,argptr);
	va_end(argptr);
	return ret;
}

/*
 */
StringStack<> VString::trim(const char *str,const char *symbols) {
	if(symbols == NULL) symbols = " ";
	const char *s = str;
	while(*s && strchr(symbols,*s) != NULL) s++;
	StringStack<> ret = StringStack<>(s);
	str = ret.get();
	s = str + ret.size() - 1;
	while(s > str && strchr(symbols,*s--) != NULL) ret.remove();
	return ret;
}

StringStack<> VString::replace(const char *str,const char *before,const char *after) {
	StringStack<> ret;
	const char *s = str;
	int m_Size = (int)strlen(before);
	while(*s) {
		if(m_Size && !strncmp(s,before,m_Size)) {
			ret += after;
			s += m_Size;
		} else {
			ret += *s++;
		}
	}
	return ret;
}

/*
 */
StringArray<> VString::split(const char *str,const char *delimiters) {
	StringArray<> ret((int)strlen(str));
	const char *s = str;
	char *d = ret.m_Str;
	int index = -1;
	while(*s) {
		if(strchr(delimiters,*s)) {
			while(*s && strchr(delimiters,*s)) {
				*d++ = '\0';
				s++;
			}
			if(index != -1) ret.append(index);
			index = -1;
		} else {
			if(*s == '\\' && *(s + 1) != '\0' && strchr(delimiters,*(s + 1))) s++;
			if(index == -1) index = (int)(d - ret.m_Str);
			*d++ = *s++;
		}
	}
	if(index != -1) ret.append(index);
	*d = '\0';
	return ret;
}

StringStack<> VString::substr(const char *str,int pos,int len) {
	StringStack<> ret;
	int m_Size = (int)strlen(str);
	if(len == -1) len = m_Size - pos;
	for(int i = 0; i < len; i++) {
		if(pos + i < 0) ret += ' ';
		else if(pos + i < m_Size) ret += str[pos + i];
		else ret += ' ';
	}
	return ret;
}

StringStack<> VString::addslashes(const char *str) {
	StringStack<> ret;
	const char *s = str;
	while(*s) {
		if(*s == '\t') ret += "\\t";
		else if(*s == '\n') ret += "\\n";
		else if(*s == '\r') ret += "\\r";
		else if(*s == '"') ret += "\\\"";
		else if(*s == '\\') ret += "\\\\";
		else ret += *s;
		s++;
	}
	return ret;
}

StringStack<> VString::stripslashes(const char *str) {
	StringStack<> ret;
	const char *s = str;
	while(*s) {
		if(*s == '\\') {
			s++;
			if(*s == 't') ret += '\t';
			else if(*s == 'n') ret += '\n';
			else if(*s == 'r') ret += '\r';
			else if(*s == '"') ret += '\"';
			else if(*s == '\\') ret += '\\';
			else if(*s == '0' && (*(s + 1) >= '0' && *(s + 1) <= '7') && (*(s + 2) >= '0' && *(s + 2) <= '7')) {
				ret += (*(s + 1) - '0') * 8 + (*(s + 2) - '0');
				s += 2;
			}
			else ret += VString::format("\\%c",*s);
			s++;
		} else {
			ret += *s++;
		}
	}
	return ret;
}

/*
 */
StringStack<> VString::dirname(const char *str) {
	const char *s = strrchr(str,'/');
	if(s == NULL) s = strrchr(str,'\\');
	if(s == NULL) return StringStack<>();
	return substr(str,0,(int)(s - str) + 1);
}

StringStack<> VString::basename(const char *str) {
	const char *s = strrchr(str,'/');
	if(s == NULL) s = strrchr(str,'\\');
	if(s == NULL) return StringStack<>(str);
	return StringStack<>(s + 1);
}

StringStack<> VString::pathname(const char *str) {
	StringStack<> ret;
	const char *s = str;
	while(1) {
		if(*s == '/' && ret.size() > 0 && ret[ret.size() - 1] == '/') {
			s++;
		} else if(!strncmp(s,"./",2)) {
			s += 2;
		} else if(!strncmp(s,"../",3)) {
			s += 3;
			if(ret.size() > 0 && ret[ret.size() - 1] == '/' && (ret.size() < 3 || strncmp(ret.get() + ret.size() - 3,"../",3))) {
				#ifdef _WIN32
					if(ret.size() != 3 || isalpha(ret[0]) == 0 || ret[1] != ':' || ret[2] != '/') {
				#else
					if(ret.size() != 1 || ret[0] != '/') {
				#endif
					ret.remove();
					int pos = ret.rfind('/');
					if(pos == -1) ret.clear();
					else ret.remove(pos + 1,ret.size() - pos - 1);
				}
			} else {
				#ifdef _WIN32
					if(ret.size() < 3 || isalpha(ret[0]) == 0 || ret[1] != ':' || ret[2] != '/') {
						ret.append("../");
					}
				#else
					if(ret.size() < 1 || ret[0] != '/') {
						ret.append("../");
					}
				#endif
			}
		} else {
			const char *slash = strchr(s,'/');
			if(slash == NULL) break;
			ret.append(s,(int)(slash - s) + 1);
			s = slash + 1;
		}
	}
	return ret;
}

StringStack<> VString::absname(const char *path,const char *str) {
	StringStack<> ret;
	#ifdef _WIN32
		const char *s = str;
		if(*s == '\0' || *(s + 1) != ':') ret = path;
	#else
		ret = path;
	#endif
	ret += str;
	return ret;
}

StringStack<> VString::relname(const char *path,const char *str) {
	StringStack<> ret;
	const char *p = path;
	const char *s = str;
	while(*p && *p == *s) {
		p++;
		s++;
	}
	while(p > path && *p != '/') p--;
	while(s > str && *s != '/') s--;
	if(*p != '/' || *s != '/') return StringStack<>(str);
	while(*p) {
		if(*++p == '/') ret += "../";
	}
	ret += s + 1;
	return ret;
}

StringStack<> VString::extension(const char *str,const char *ext) {
	StringStack<> ret(str);
	int pos = ret.rfind('.');
	if(pos != -1 && pos > ret.rfind('/') && pos > ret.rfind('\\')) {
		if(lower(substr(ret,pos)) != ext) {
			ret.remove(pos,ret.size() - pos);
			ret += ext;
		}
	} else {
		ret += ext;
	}
	return ret;
}

/******************************************************************************\
*
* VString unicode functions
*
\******************************************************************************/

/*
 */
StringStack<> VString::unicodeToUtf8(const wchar_t *src) {
	StringStack<> ret;
	const wchar_t *s = src;
	while(*s) {
		unsigned int code = *s++;
		if(code < 0x80) {
			ret += code & 0x7f;
		} else if(code < 0x800) {
			ret += 0xc0 | (code >> 6);
			ret += 0x80 | (code & 0x3f);
		} else if(code < 0x10000) {
			ret += 0xe0 | (code >> 12);
			ret += 0x80 | ((code >> 6) & 0x3f);
			ret += 0x80 | (code & 0x3f);
		}
	}
	return ret;
}

StringStack<> VString::unicodeToUtf8(const unsigned int *src) {
	StringStack<> ret;
	const unsigned int *s = src;
	while(*s) {
		unsigned int code = *s++;
		if(code < 0x80) {
			ret += code & 0x7f;
		} else if(code < 0x800) {
			ret += 0xc0 | (code >> 6);
			ret += 0x80 | (code & 0x3f);
		} else if(code < 0x10000) {
			ret += 0xe0 | (code >> 12);
			ret += 0x80 | ((code >> 6) & 0x3f);
			ret += 0x80 | (code & 0x3f);
		} else if(code < 0x200000) {
			ret += 0xf0 | (code >> 18);
			ret += 0x80 | ((code >> 12) & 0x3f);
			ret += 0x80 | ((code >> 6) & 0x3f);
			ret += 0x80 | (code & 0x3f);
		}
	}
	return ret;
}

int VString::utf8ToUnicode(const char *src,unsigned int &code) {
	const unsigned char *s = (const unsigned char*)src;
	code = *s;
	if(code > 0xf0) {
		code = (unsigned int)(*s++ & 0x07) << 18;
		if(*s) code |= (unsigned int)(*s++ & 0x3f) << 12;
		if(*s) code |= (unsigned int)(*s++ & 0x3f) << 6;
		if(*s) code |= (unsigned int)(*s++ & 0x3f);
	} else if(code > 0xe0) {
		code = (unsigned int)(*s++ & 0x0f) << 12;
		if(*s) code |= (unsigned int)(*s++ & 0x3f) << 6;
		if(*s) code |= (unsigned int)(*s++ & 0x3f);
	} else if(code > 0xc0) {
		code = (unsigned int)(*s++ & 0x1f) << 6;
		if(*s) code |= (unsigned int)(*s++ & 0x3f);
	} else {
		s++;
	}
	return (int)(s - (const unsigned char*)src);
}

int VString::utf8ToUnicode(const char *src,unsigned int *dest,int size) {
	const char *s = src;
	unsigned int *d = dest;
	unsigned int code = 0;
	while(*s && --size) {
		s += utf8ToUnicode(s,code);
		*d++ = code;
	}
	*d = '\0';
	return (int)(d - dest);
}

int VString::utf8ToUnicode(const char *src,wchar_t *dest,int size) {
	const char *s = src;
	wchar_t *d = dest;
	unsigned int code = 0;
	while(*s && --size) {
		s += utf8ToUnicode(s,code);
		if(code < 0x10000) *d++ = (wchar_t)code;
		else *d++ = 0x25a1;
	}
	*d = '\0';
	return (int)(d - dest);
}

/*
 */
int VString::utf8strlen(const char *str) {
	const unsigned char *s = (const unsigned char*)str;
	int m_Size = 0;
	while(*s) {
		int code = *s++;
		if(code > 0xf0) {
			if(*s) s++;
			if(*s) s++;
			if(*s) s++;
		} else if(code > 0xe0) {
			if(*s) s++;
			if(*s) s++;
		} else if(code > 0xc0) {
			if(*s) s++;
		}
		m_Size++;
	}
	return m_Size;
}

StringStack<> VString::utf8substr(const char *str,int pos,int len) {
	StringStack<> ret;
	const unsigned char *s = (const unsigned char*)str;
	int m_Size = 0;
	while(*s) {
		if(m_Size == pos) str = (const char*)s;
		int code = *s++;
		if(code > 0xf0) {
			if(*s) s++;
			if(*s) s++;
			if(*s) s++;
		} else if(code > 0xe0) {
			if(*s) s++;
			if(*s) s++;
		} else if(code > 0xc0) {
			if(*s) s++;
		}
		m_Size++;
	}
	s = (const unsigned char*)str;
	if(len == -1) len = m_Size - pos;
	for(int i = 0; i < len; i++) {
		if(pos + i < 0) ret += ' ';
		else if(pos + i < m_Size) {
			int code = *s++;
			ret += (char)code;
			if(code > 0xf0) {
				if(*s) ret += *s++;
				if(*s) ret += *s++;
				if(*s) ret += *s++;
			} else if(code > 0xe0) {
				if(*s) ret += *s++;
				if(*s) ret += *s++;
			} else if(code > 0xc0) {
				if(*s) ret += *s++;
			}
		}
		else ret += ' ';
	}
	return ret;
}

/******************************************************************************\
*
* VString compare functions
*
\******************************************************************************/

/*
 */
int VString::match(const char *pattern,const char *str) {
	int ret = 1;
	const char *p = pattern;
	const char *s = str;
	while(*p) {
		if(*p == '*' && ret) {
			if(*(p + 1) == '\0' || *(p + 1) == '|') break;
			p++;
			int len = 0;
			while(p[len] && p[len] != '|') len++;
			while(*s) s++;
			if(s - str >= len) s -= len;
		} else if(*p == '|') {
			if(ret) break;
			p++;
			s = str;
			ret = 1;
		} else {
			if(*p != '?' && *p != *s) ret = 0;
			if(*s) s++;
			p++;
		}
	}
	if(*s != '\0' && *p == '\0') ret = 0;
	return ret;
}

#endif 


const char* VStrIStr(const char* Str, const char* Sub)
{
	if( Str == NULL || Sub == NULL || *Sub == 0)
	{
		return NULL;
	}
	// 获得第一个字符, 首先查找第一个字符所在位置.
	char FirstChar = gc_LowerTable[*Sub];
	// 剩余字符的个数
	int   Length = strlen(Sub++) - 1;
	char StrChar = *Str++;
	while (StrChar)
	{
		// 转换为小写
		StrChar = gc_LowerTable[StrChar];
		// 如果当前字符和要查找的第一个字符相等, 比较剩余的查找字符
		if (StrChar == FirstChar && !VStrnicmp(Str, Sub, Length))
		{
			// 找到字符串
			return Str-1;
		}
		// 下一个字符
		StrChar = *Str++;
	}

	// 没有找到字符串
	return NULL;
}

VString VString::SubStr(const char* String, int Pos, int Count)
{
	if (String == NULL || *String == 0)
	{
		return VString(NULL);
	}
	int Len = strlen(String);
	if (Count == -1)
	{
		Count = Len - Pos;
	}
	if (Count <= 0)
	{
		return VString(NULL);
	}

	int End = Pos+Count;
	Pos = VClamp(Pos, 0, Len);
	End = VClamp(End, Pos, Len);
	return VString(String + Pos, End - Pos);
}

VString VString::Replace(const char* String, const char* From, const char* To, BOOL IgnoreCace)
{
	if (String == NULL || *String == 0)
	{
		return VString(NULL);
	}
}

VString VString::Replace(const char* From, const char* To, BOOL IgnoreCase) const
{
	if (Empty())
	{
		return *this;
	}
	
	INT FromLength = strlen(From);
	if (FromLength == 0)
	{
		return *this;
	}

	VString Result;
	char* Str = m_Elements;

	

	while (TRUE)
	{
		// look for From in the remaining string
		char* Found = IgnoreCase ? VStrIStr(Str, From) : VStrStr(Str, From);
		if (Found)
		{
			// replace the first letter of the From with 0 so we can do a strcpy (FString +=)
			char C = *Found;
			*Found = 0;

			// copy everything up to the From
			Result += Str;

			// copy over the To
			Result += To;

			// retore the letter, just so we don't have 0's in the string
			*Found = *From;

			Str = Found + FromLength;
		}
		else
		{
			break;
		}
	}

	// copy anything left over
	Result += Str;

	return Result;
}

int VString::ReplaceFast(const char* From, const char* To, BOOL IgnoreCase)
{
	if (Empty())
	{
		return 0;
	}
	VASSERT(From && To);
	UINT FromLength = strlen(From);
	UINT ToLength = strlen(To);
	VASSERT(FromLength >= ToLength);
	if (FromLength == 0 )
	{
		return 0;
	}
	UINT NumToRemove = FromLength - ToLength;

	char* Str = m_Elements;
	
	if (ToLength)
	{
		while (1)
		{
			// look for From in the remaining string
			char* Found = IgnoreCase ? VStrIStr(Str, From) : VStrStr(Str, From);
			if (Found)
			{
				if (NumToRemove)
				{
					EraseAt((UINT)(Found - Str), NumToRemove);
				}
				for (UINT Index = 0; Index < ToLength; ++Index)
				{
					Found[Index] = To[Index];
				}

				Str = Found + FromLength;
			}
			else
			{
				break;
			}
		}
	}else
	{
		while (1)
		{
			// look for From in the remaining string
			char* Found = IgnoreCase ? VStrIStr(Str, From) : VStrStr(Str, From);
			if (Found)
			{
				if (NumToRemove)
				{
					EraseAt((UINT)(Found - Str), NumToRemove);
				}
				Str = Found + FromLength;
			}
			else
			{
				break;
			}
		}
	}
	m_Size = strlen(m_Elements) + 1;
}


UINT VString::Split(const char* String, const char* Delimiters, VPODArray<SplitResult>& SpliteResults)
{
	SpliteResults.Clear();
	if (String == NULL || *String == 0)
	{
		return 0;
	}
	
	const char* s = String;
	SplitResult Result;
	Result.Start = NULL;
	int index = -1;
	while(*s) 
	{
		// 检查当前字符是否是分隔符
		if(strchr(Delimiters,*s)) 
		{
			if (Result.Start != NULL)
			{
				Result.End = s;
				SpliteResults.PushBack(Result);
				Result.Start = NULL;
			}

			// 忽略接下来的所有分隔符
			while(*s && strchr(Delimiters,*s)) 
			{
				s++;
			}
		} else 
		{
			// 当前字符不是分隔符
			if (Result.Start == NULL)
			{
				Result.Start = s;
			}
			s++;
		}
	}
	if (Result.Start != NULL)
	{
		Result.End = s;
		SpliteResults.PushBack(Result);
	}
	return SpliteResults.Size();
}

VArray<VString> VString::Split(const char* String, const char* Delimiters)
{
	VPODArray<SplitResult> SpliteResults;
	Split(String, Delimiters, SpliteResults);
	VArray<VString> SubStrings;

	for (UINT Index = 0; Index < SpliteResults.Size(); ++Index)
	{
		SplitResult& Result = SpliteResults[Index];
		SubStrings.PushBack(VString(Result.Start, Result.End - Result.Start));
	}
	return SubStrings;
}
//
//VString VString::Trim(const char* Symbols) const
//{
//	if(Symbols == NULL) 
//	{ 
//		Symbols = " ";
//	}
//	VString Result;
//	if (Empty())
//	{
//		return Result;
//	}
//	
//	const char *s = m_Elements;
//	while(*s && strchr(Symbols,*s) != NULL) 
//	{
//		s++;
//	}
//	StringStack<> ret = StringStack<>(s);
//	str = ret.get();
//	s = str + ret.size() - 1;
//	while(s > str && strchr(symbols,*s--) != NULL) ret.remove();
//	return ret;
//}