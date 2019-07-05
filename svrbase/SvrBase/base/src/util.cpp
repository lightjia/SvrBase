#include "util.h"
#include "MemMgr.h"

void* do_malloc(size_t iLen)
{
	if (iLen <= 0)
	{
		return NULL;
	}

	void* pRet = NULL;
	while (NULL == pRet)
	{
		pRet = malloc(iLen);
	}

	memset(pRet, 0, iLen);
	
	return pRet;
}

uint64_t get_time_ms()
{
	uint64_t iRet = 0;
#if (defined PLATFORM_WINDOWS)
	iRet = GetTickCount();
#elif  (defined PLATFORM_LINUX)
	struct timeval current;
	gettimeofday(&current, NULL);
	iRet = (uint64_t)(current.tv_sec * 1000) + current.tv_usec/1000;
#endif

	return iRet;
}

void sleep_ms(DWORD dwMillions)
{
#if (defined PLATFORM_WINDOWS)
	Sleep(dwMillions);
#elif  (defined PLATFORM_LINUX)
	usleep(dwMillions * 1000);
#endif
}

struct systemtime_t get_now_time()
{
	struct systemtime_t stRet;
	time_t rawtime;
	struct tm tmnow;
#if (defined PLATFORM_WINDOWS)
	time(&rawtime);
	localtime_s(&tmnow, &rawtime);
	struct timeb tp;
	ftime(&tp);
	stRet.tmmilliseconds = tp.millitm;
#elif  (defined PLATFORM_LINUX)
	struct timeval tv_now;
	gettimeofday(&tv_now, NULL);
	rawtime = (time_t)tv_now.tv_sec;
	
	localtime_r(&rawtime, &tmnow);
	stRet.tmmilliseconds = static_cast<int>(tv_now.tv_usec);
#endif

	stRet.tmyear = tmnow.tm_year + 1900;
	stRet.tmmon = tmnow.tm_mon + 1;
	stRet.tmmday = tmnow.tm_mday;
	stRet.tmhour = tmnow.tm_hour;
	stRet.tmmin = tmnow.tm_min;
	stRet.tmsec = tmnow.tm_sec;
	stRet.tmwday = tmnow.tm_wday;
	stRet.tmyday = tmnow.tm_yday;
	stRet.tmisdst = tmnow.tm_isdst;
	
	return stRet;
}

DWORD get_thread_id_self()
{
	DWORD iRet = 0;
#if (defined PLATFORM_WINDOWS)
	iRet = ::GetCurrentThreadId();
#elif  (defined PLATFORM_LINUX)
	iRet = pthread_self();
#endif

	return iRet;
}

long atomic_change(volatile long* value, int off)
{
  long lRet = 0;
	if (NULL != value)
	{
#if (defined PLATFORM_WINDOWS)
    lRet = InterlockedExchangeAdd(value, off);
#elif  (defined PLATFORM_LINUX)
    lRet = __sync_fetch_and_add(value, off);
#endif
	}

  return lRet;
}

int split_str(std::string strSource, std::string strSep, std::vector<std::string>& vecRet)
{
    size_t i = 0;
    vecRet.clear();
    while (i < strSource.size()) {
        size_t iPos = strSource.find(strSep, i);
        if (iPos == std::string::npos) {
            vecRet.push_back(strSource.substr(i));
            break;
        }
        else {
            vecRet.push_back(strSource.substr(i, iPos - i));
            i = iPos + strSep.size();
        }
    }

    return (int)vecRet.size();
}

bool str_cmp(const char* pStr1, const char* pStr2, bool bIgnoreCase)
{
	if (pStr1 == pStr2)
	{
		return true;
	}

	if (NULL == pStr1 || NULL == pStr2)
	{
		return false;
	}

	size_t iLen = strlen(pStr1);
	if (iLen != strlen(pStr2))
	{
		return false;
	}

	for (int i = 0; i <= iLen; ++i)
	{
		char c1 = pStr1[i];
		char c2 = pStr2[i];
		if (bIgnoreCase)
		{
			c1 = toupper(c1);
			c2 = toupper(c2);
		}

		if (c1 != c2)
		{
			return false;
		}
	}

	return true;
}

bool str_start_with(const std::string& str, const std::string& prefix)
{
	if (prefix.length() > str.length())
	{
		return false;
	}

	if (memcmp(str.c_str(), prefix.c_str(), prefix.length()) == 0) 
	{
		return true;
	}

	return false;
}

bool str_end_with(const std::string& str, const std::string& suffix)
{
	if (suffix.length() > str.length())
	{
		return false;
	}

	return (str.substr(str.length() - suffix.length()) == suffix);
}

std::string& str_ltrim(std::string& str) 
{ // NOLINT
	std::string::iterator it = find_if(str.begin(), str.end(), (int(*)(int))isspace);
	str.erase(str.begin(), it);
	return str;
}

std::string& str_rtrim(std::string& str) 
{ // NOLINT
	std::string::reverse_iterator it = find_if(str.rbegin(),
		str.rend(), (int(*)(int))isspace);

	str.erase(it.base(), str.end());
	return str;
}

std::string& str_trim(std::string& str) 
{ // NOLINT
	return str_rtrim(str_ltrim(str));
}

std::string str_2_lower(std::string& str)
{
	std::string strRet = str;
	transform(strRet.begin(), strRet.end(), strRet.begin(), (int(*)(int))tolower);
	return strRet;
}

std::string str_2_upper(std::string& str)
{
	std::string strRet = str;
	transform(strRet.begin(), strRet.end(), strRet.begin(), (int(*)(int))toupper);
	return strRet;
}

bool str_cmp_nocase(std::string str1, std::string str2)
{
	std::string strTmp1 = str_2_upper(str1);
	std::string strTmp2 = str_2_upper(str2);

	return strTmp1 == strTmp2;
}

wchar_t* char_2_wchar(const char* pStr)
{
	wchar_t* pRet = NULL;
	if (NULL != pStr)
	{
		size_t iLen = strlen(pStr);
		if (iLen > 0)
		{
			iLen += 1;
			pRet = (wchar_t*)do_malloc(iLen * sizeof(wchar_t));
			if (NULL != pRet)
			{
#if (defined PLATFORM_WINDOWS)
				size_t converted = 0;
				mbstowcs_s(&converted, pRet, iLen, pStr, _TRUNCATE);
#elif  (defined PLATFORM_LINUX)
				mbstowcs(pRet, pStr, iLen - 1);
#endif	
			}	
		}
	}

	return pRet;
}

char* wchar_2_char(const wchar_t* pStr)
{
	char* pRet = NULL;
	if (NULL != pStr)
	{
		size_t iLen = wcslen(pStr);
		if (iLen > 0)
		{
			iLen += 1;
			pRet = (char*)do_malloc(iLen * sizeof(char));
			if (NULL != pRet)
			{
#if (defined PLATFORM_WINDOWS)
				size_t converted = 0;
				wcstombs_s(&converted, pRet, iLen, pStr, _TRUNCATE);
#elif  (defined PLATFORM_LINUX)
				wcstombs(pRet, pStr, iLen - 1);
#endif	
			}
		}
	}

	return pRet;
}

const long get_file_len(const char* pFileName, const char* pMode)
{
	long lRet = 0;
	FILE *pFile = safe_open_file(pFileName, pMode);
	if (NULL != pFile)
	{
		fseek(pFile, 0L, SEEK_END);
		lRet = ftell(pFile);
		fclose(pFile);
	}
	
	return lRet;
}

size_t file_write(const char* pFileName, const char* pMode, const unsigned char* pData, const size_t iLen)
{
	size_t iWrite = 0;
	do
	{
		if (NULL == pData || iLen <= 0)
		{
			break;
		}

		FILE *pFile = safe_open_file(pFileName, pMode);
		if (NULL != pFile)
		{
			while (iWrite < iLen)
			{
				iWrite += fwrite((void*)(pData + iWrite), sizeof(unsigned char), iLen - iWrite, pFile);
			}
			fclose(pFile);
		}
	} while (0);

	return iWrite;
}

size_t file_read(const char* pFileName, const char* pMode, unsigned char* pData, const size_t iLen, const long lOffset)
{
	size_t iRead = 0;
	do
	{
		if (NULL == pData || iLen <= 0)
		{
			break;
		}

		FILE *pFile = safe_open_file(pFileName, pMode);
		if (NULL != pFile)
		{
			if (fseek(pFile, lOffset, SEEK_SET) != 0)
			{
				break;
			}

			while (iRead < iLen && !feof(pFile))
			{
				iRead += fread((void*)(pData + iRead), sizeof(unsigned char), 1, pFile);
			}
			fclose(pFile);
		}
	} while (0);

	return iRead;
}

char* file_read(const char* pFileName, const char* pMode, size_t& lLen)
{
	char* pRet = NULL;
	lLen = get_file_len(pFileName, pMode);
	if (lLen > 0)
	{
		pRet = (char*)do_malloc(lLen);
		if (pRet != NULL)
		{
			FILE *pFile = safe_open_file(pFileName, pMode);
			if (NULL != pFile)
			{
				size_t iRead = 0;
				while (iRead < lLen && !feof(pFile))
				{
					iRead += fread((void*)(pRet + iRead), sizeof(char), lLen - iRead, pFile);
				}
				fclose(pFile);
			}
		}
	}

	return pRet;
}

bool file_exits(const char*  strFileName)
{
	bool bRet = false;
#if (defined PLATFORM_WINDOWS)
	DWORD dwRet = GetFileAttributesA(strFileName);
	bRet = (dwRet != INVALID_FILE_ATTRIBUTES) && (!(dwRet & FILE_ATTRIBUTE_DIRECTORY));
#elif  (defined PLATFORM_LINUX)
	bRet = (access(strFileName, F_OK) == 0);
#endif
	return bRet;
}

bool directory_exists(const char* strDirectory)
{
	bool bRet = false;
#if (defined PLATFORM_WINDOWS)
	DWORD dwRet = GetFileAttributesA(strDirectory);
	bRet = (dwRet != INVALID_FILE_ATTRIBUTES) && (dwRet & FILE_ATTRIBUTE_DIRECTORY);
#elif  (defined PLATFORM_LINUX)
	struct stat sb;
	bRet = (lstat(strDirectory, &sb) == 0 && S_ISDIR(sb.st_mode));
#endif
	return bRet;
}

std::string get_app_path()
{
	std::string strRet = "";
#if (defined PLATFORM_WINDOWS)
	char szExePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szExePath, MAX_PATH);
	char *pstr = strrchr(szExePath, '\\');
	memset(pstr + 1, '\0', 1);
	strRet = szExePath;
#elif  (defined PLATFORM_LINUX)
#define MAX_PATH 0x100
	char szFilePath[MAX_PATH] = { 0 };
	int cnt = readlink("/proc/self/exe", szFilePath, MAX_PATH);
	if (cnt < 0 || cnt >= MAX_PATH)
	{
		return strRet;
	}

	for (int i = cnt; i >= 0; --i)
	{
		if (szFilePath[i] == '/')
		{
			szFilePath[i + 1] = '\0';
			break;
		}
	}
	strRet = szFilePath;
#endif
	
	return strRet;
}

FILE* safe_open_file(const char* pFileName, const char* pMode)
{
	FILE* pRet = NULL;
#if (defined PLATFORM_WINDOWS)
	/*if (0 != fopen_s(&pRet, pFileName, pMode))
	{
		pRet = NULL;
	}*/
	//fopen_s(&pRet,pFileName, pMode);
    pRet = fopen(pFileName, pMode);
#elif  (defined PLATFORM_LINUX)
	pRet = fopen(pFileName, pMode);
#endif

	//设置非缓冲
	if (NULL != pRet)
	{
		setvbuf(pRet, NULL, _IONBF, 0);
	}

	return pRet;
}

int rm_file(const char* pFileName) {
	if (strlen(pFileName) <= 0) {
		return 1;
	}

	int iRet;
#if (defined PLATFORM_WINDOWS)
	iRet = DeleteFileA(pFileName);
#elif  (defined PLATFORM_LINUX)
	iRet = unlink(pFileName);
#endif
	return iRet;
}

int rm_dir(const char* pDirName) {
	if (!directory_exists(pDirName)) {
        fprintf(stderr, "Dir:%s Not Exist!\n", pDirName);
		return 1;
	}

#if (defined PLATFORM_WINDOWS)
    std::string strPath = pDirName;
    if (str_end_with(strPath, "\\") || str_end_with(strPath, "/")) {
        strPath += "*";
    } else {
        strPath += "/*";
    }

    WIN32_FIND_DATAA FindFileData;
    ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATAA));
    HANDLE hFile = FindFirstFileA(strPath.c_str(), &FindFileData);
    if (hFile && INVALID_HANDLE_VALUE != hFile) {
        BOOL IsFinded = FALSE;
        do {
            if (IsFinded) {
                if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, "..")) {//如果不是"." ".."目录
                    std::string strFileName = "";
                    strFileName = strFileName + pDirName + "/" + FindFileData.cFileName;
                    if (directory_exists(strFileName.c_str())) {//如果是目录，则递归地调用
                        rm_dir(strFileName.c_str());
                    }
                    else {
                        rm_file(strFileName.c_str());
                    }
                }
            }

            IsFinded = FindNextFileA(hFile, &FindFileData);    //递归搜索其他的文件
        } while (IsFinded);

        FindClose(hFile);
    }
    RemoveDirectoryA(pDirName);
#elif  (defined PLATFORM_LINUX)
    DIR *pDir = opendir(pDirName);
    if (pDir) {
        struct dirent *dmsg = NULL;
        while ((dmsg = readdir(pDir)) != NULL){
            if (strcmp(dmsg->d_name, ".") && strcmp(dmsg->d_name, "..")){
                std::string strFileName = "";
                strFileName = strFileName + pDirName + "/" + dmsg->d_name;
                if (directory_exists(strFileName.c_str())) {//如果是目录，则递归地调用
                    rm_dir(strFileName.c_str());
                } else {
                    rm_file(strFileName.c_str());
                }
            }
        }

        closedir(pDir);
    }

    rmdir(pDirName);
#endif

	return 0;
}

len_str get_bmp(const void* pBmp, int width, int height, int bitCount)
{
	len_str stImg;
    BZERO(stImg);
	if (NULL == pBmp)
	{
		return stImg;
	}

	int bmp_size = width*height*(bitCount / 8);

	// 【写位图文件头】
	BITMAPFILEHEADER bmpHeader;
	bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp_size;    // BMP图像文件的大小
	bmpHeader.bfType = 0x4D42;    // 位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);            // BMP图像数据的偏移位置
	bmpHeader.bfReserved1 = 0;    // 总为0
	bmpHeader.bfReserved2 = 0;    // 总为0

	stImg.iLen = bmpHeader.bfSize;
	stImg.pStr = (char*)do_malloc(sizeof(char)*bmpHeader.bfSize);
	if (NULL == stImg.pStr)
	{
		stImg.iLen = 0;
		return stImg;
	}

	size_t iOffset = 0;
	memcpy(stImg.pStr, &bmpHeader, sizeof(bmpHeader));
	iOffset += sizeof(bmpHeader);

	BITMAPINFOHEADER bmiHeader;
	bmiHeader.biSize = sizeof(bmiHeader);                // 本结构所占用字节数，即sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;                            // 位图宽度（单位：像素）
	bmiHeader.biHeight = height;                        // 位图高度（单位：像素）
	bmiHeader.biPlanes = 1;                                // 目标设备的级别，必须为1
	bmiHeader.biBitCount = bitCount;                    // 像素的位数（每个像素所需的位数，范围：1、4、8、24、32）
	bmiHeader.biCompression = 0;                        // 压缩类型（0：不压缩 1：BI_RLE8压缩类型 2：BI_RLE4压缩类型）
	bmiHeader.biSizeImage = bmp_size;                    // 位图大小（单位：字节）
	bmiHeader.biXPelsPerMeter = 0;                        // 水平分辨率(像素/米)
	bmiHeader.biYPelsPerMeter = 0;                        // 垂直分辨率(像素/米)
	bmiHeader.biClrUsed = 0;                            // 位图实际使用的彩色表中的颜色索引数
	bmiHeader.biClrImportant = 0;                        // 对图象显示有重要影响的颜色索引的数目

	// 【写位图信息头（BITMAPINFO的bmiHeader成员）】
	memcpy(stImg.pStr + iOffset, &bmiHeader, sizeof(bmiHeader));
	iOffset += sizeof(bmiHeader);

	// 【写像素内容】
	memcpy(stImg.pStr + iOffset, pBmp, bmp_size);

	return stImg;
}

int make_dir(const char* path)
{
	if (NULL == path || strlen(path) <= 0)
	{
		return 1;
	}

	if(directory_exists(path))
	{
		return 0;
	}

#if (defined PLATFORM_WINDOWS)
	if (!::CreateDirectoryA(path, NULL))
	{
		return 1;
	}
#elif  (defined PLATFORM_LINUX)
	if (mkdir(path, 0755) != 0)
	{
		fprintf(stderr, "mkdir %s failed(%s)\n", path, strerror(errno));
		return 1;
	}
#endif
	
	return 0;
}

int make_dirs(const char* path)
{
	size_t len = 0;
	if (NULL == path || (len = strlen(path)) <= 0)
	{
		return 1;
	}

	char* pTmp = (char*)sMemMgr->MemMalloc(len + 3);
	if (NULL == pTmp)
	{
		return 1;
	}

	snprintf(pTmp, len + 1, "%s", path);

	for (int i = 0; i < (int)len; i++) 
	{
		if (pTmp[i] != '\\' && pTmp[i] != '/')
		{
			continue;
		}

		if (0 == i) {
			continue;
		}

		pTmp[i] = '\0';
		if (make_dir(pTmp) != 0)
		{
			break;
		}

		pTmp[i] = '/';
	}

	sMemMgr->MemFree(pTmp);

	return make_dir(path);
}

int init_platform()
{
	int iRet = 0;
#if (defined PLATFORM_WINDOWS)
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		iRet = 1;
	}
	_setmode(0, _O_BINARY);
	_setmode(1, _O_BINARY);
	_setmode(2, _O_BINARY);
#elif  (defined PLATFORM_LINUX)
    signal(SIGPIPE, SIG_IGN);
#endif

	//输出非缓冲
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	return iRet;
}

int uninit_platform()
{
#if (defined PLATFORM_WINDOWS)
	WSACleanup();
#elif  (defined PLATFORM_LINUX)
#endif

	return 0;
}

void perror_desc(const char* pStr)
{
#if (defined PLATFORM_WINDOWS)
	wchar_t* pWstr = char_2_wchar(pStr);
	if (NULL == pWstr)
	{
		return;
	}

	wchar_t* pBuf = NULL;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		pBuf,
		0,
		NULL))
	{
		wchar_t* pFormat = char_2_wchar("%s: %s");
		if (NULL != pFormat)
		{
			fwprintf_s(stderr, pFormat, pWstr, pBuf);
			fflush(stderr);
			DOFREE(pFormat);
		}
		LocalFree((HLOCAL)pBuf);
	}
	else
	{
		wchar_t* pFormat = char_2_wchar("%s: unknown Windows error\n");
		if (NULL != pFormat)
		{
			fwprintf_s(stderr, pFormat, pWstr);
			fflush(stderr);
			DOFREE(pFormat);
		}
	}

	DOFREE(pWstr);
#elif  (defined PLATFORM_LINUX)
	if (NULL != pStr)
	{
		perror(pStr);
	}
#endif
}