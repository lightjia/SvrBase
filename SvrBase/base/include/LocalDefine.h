#ifndef __LOCALDEFINE__H_
#define __LOCALDEFINE__H_
#include "common.h"
#if (defined PLATFORM_WINDOWS)
#elif  (defined PLATFORM_LINUX)
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef long LONG;
#define INFINITE            0xFFFFFFFF

typedef struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER;
#endif


#if (defined PLATFORM_WINDOWS)
#elif  (defined PLATFORM_LINUX)
#define _TRUNCATE ((size_t)-1)
#define _strdup	strdup
#endif

struct systemtime_t
{
	int tmsec;     /* seconds after the minute - [0,59] */
	int tmmin;     /* minutes after the hour - [0,59] */
	int tmhour;    /* hours since midnight - [0,23] */
	int tmmday;    /* day of the month - [1,31] */
	int tmmon;     /* months since January - [0,11] */
	int tmyear;    /* years since 1900 */
	int tmwday;    /* days since Sunday - [0,6] */
	int tmyday;    /* days since January 1 - [0,365] */
	int tmisdst;   /* daylight savings time flag */
	int tmmilliseconds;/*milliseconds after the sec[0,1000]*/
};

#pragma pack(1)
struct len_str
{
	char* pStr;
	size_t iLen;
};
#pragma pack()
#endif