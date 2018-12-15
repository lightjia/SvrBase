#include "CLogmanager.h"

#define RESET           0  
#define BRIGHT          1  
#define DIM             2  
#define UNDERLINE       4  
#define BLINK           5  
#define REVERSE         7  
#define HIDDEN          8  

#define BLACK           0  
#define RED             1  
#define GREEN           2  
#define YELLOW          3  
#define BLUE            4  
#define MAGENTA         5  
#define CYAN            6  
#define WHITE           7 

#if (defined PLATFORM_WINDOWS)
#elif  (defined PLATFORM_LINUX)
#define COLOR_PRINT_BEGIN	\033
#define COLOR_PRINT_END	\033[0m
#endif

CLogmanger::CLogmanger()
{
	miLevel = LOG_LEVEL_ERR;
	miType = LOG_TYPE_SCREEN;
	mlCount = 0;
	mstrDir = "";
	mbInit = false;
	mpFile = stdout;
	mpTmpFile = nullptr;
  mpLogCb = nullptr;
}

CLogmanger::~CLogmanger()
{
	if (nullptr != mpFile)
	{
		fclose(mpFile);
		mpFile = nullptr;
	}

	if (nullptr != mpTmpFile)
	{
		fclose(mpTmpFile);
		mpTmpFile = nullptr;
	}
}

FILE* CLogmanger::GetFile()
{
	if (mpTmpFile != nullptr  && mpTmpFile != stdout)
	{
		fclose(mpTmpFile);
		mpTmpFile = nullptr;
	}

	mpTmpFile = mpFile;
	if (miType == LOG_TYPE_FILE || miType == LOG_TYPE_TEE)
	{
		struct systemtime_t stNow = get_now_time();
		char szFileName[1000];
		safe_snprintf(szFileName, 1000, _TRUNCATE, "%02d-%02d-%02d-%02d-%02d-%02d.log", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec);
		std::string strTmp = mstrDir;
		if (!str_end_with(strTmp, "\\") && !str_end_with(strTmp, "/"))
		{
			strTmp += "/";
		}

		strTmp += szFileName;
		FILE* pTmpTile = safe_open_file(strTmp.c_str(), "a+");
		if (nullptr == pTmpTile)
		{
			fprintf(stderr, "open  log file err:%s", strTmp.c_str());
			mpFile = stdout;
		}
		else
		{
			mpFile = pTmpTile;
		}
	}
	else
	{
		mpFile = stdout;
	}
	
	return mpFile;
}

int CLogmanger::Check()
{
	//文件过大创建新文件
	if (miType == LOG_TYPE_FILE || miType == LOG_TYPE_TEE)
	{
		if (mlCount >= MAX_PER_LOGFILE_SIZE)
		{
			mcMutex.Lock();
			if (mlCount >= MAX_PER_LOGFILE_SIZE)
			{
				mlCount = 0;
				GetFile();
			}
			mcMutex.UnLock();
		}
	}

	return 0;
}

int CLogmanger::SetLogType(int iType)
{
	if (!mbInit || iType < LOG_TYPE_SCREEN || iType >= LOG_TYPE_MAX)
	{
		return -1;
	}

	if (iType == miType)
	{
		return 0;
	}

	miType = iType;
	GetFile();

	return 0;
}

int CLogmanger::SetLogLevel(int iLevel)
{
	if (!mbInit || iLevel < LOG_LEVEL_ERR)
	{
		return -1;
	}

	miLevel = iLevel;

	return 0;
}

int CLogmanger::SetLogPath(const char* pPath)
{
	if (!mbInit || nullptr == pPath || str_cmp(pPath, mstrDir.c_str(), true))
	{
		return 1;
	}

	mstrDir = pPath;
	GetFile();
	return 0;
}

void CLogmanger::AddLogItem(int iLevel, const char *format, ...)
{
  if (nullptr != mpLogCb)
  {
    va_list arg;
    va_start(arg, format);
    int nPos = 0;
    char szBuf[MAX_PER_LINE_LOG];
#if (defined PLATFORM_WINDOWS)
    nPos += vsnprintf_s(szBuf + nPos, MAX_PER_LINE_LOG - nPos - 1, _TRUNCATE, format, arg);
#elif  (defined PLATFORM_LINUX)
    nPos += vsnprintf(szBuf + nPos, MAX_PER_LINE_LOG - nPos - 1, format, arg);
#endif

    if (nPos > MAX_PER_LINE_LOG - 1)
    {
      nPos = MAX_PER_LINE_LOG - 1;
    }

    if (nPos > 0)
    {
      szBuf[nPos] = '\0';
    }

    va_end(arg);
    mpLogCb(iLevel, szBuf);
    return;
  }

	if (iLevel > miLevel || iLevel > LOG_LEVEL_MAX || !mbInit)
	{
		return;
	}

	char szBuf[MAX_PER_LINE_LOG];
	memset(szBuf, 0, sizeof(szBuf));
	int nPos = 0;
	if (iLevel <= LOG_LEVEL_INFO)
	{
		struct systemtime_t stNow = get_now_time();
		nPos = safe_snprintf(szBuf, MAX_PER_LINE_LOG - 1, _TRUNCATE, "[%02d/%02d/%02d %02d:%02d:%02d.%06d] ", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec, stNow.tmmilliseconds);
	}
	
	va_list ap;
	va_start(ap, format);
#if (defined PLATFORM_WINDOWS)
	nPos += vsnprintf_s(szBuf + nPos, MAX_PER_LINE_LOG - nPos - 1, _TRUNCATE, format, ap);
#elif  (defined PLATFORM_LINUX)
	nPos += vsnprintf(szBuf + nPos, MAX_PER_LINE_LOG - nPos - 1, format, ap);
#endif
	va_end(ap);

	if (nPos > MAX_PER_LINE_LOG - 1)
	{
		nPos = MAX_PER_LINE_LOG - 1;
	}
		
	if (nPos > 0)
	{
		szBuf[nPos] = '\0';
	}

	tagLogItem stLog;
	stLog.iLevel = iLevel;
	stLog.strLog = szBuf;
	PrintItem(stLog);
}

int CLogmanger::Init(int iType, int iLevel, const char* szDir, log_cb pLogCb)
{
  if (nullptr != pLogCb)
  {
    mpLogCb = pLogCb;
    return 0;
  }

	if (mbInit)
	{
		return 1;
	}

	miType = iType;
	miLevel = iLevel;
	if (nullptr != szDir && strlen(szDir) > 0 && !str_cmp(szDir, ".", true))
	{
		mstrDir = szDir;
	}
	else
	{
		mstrDir = get_app_path();
		mstrDir += "log";
	}

	if (make_dirs(mstrDir.c_str()))
	{
		fprintf(stderr, "make_dirs error!");
	}
	
	GetFile();

	if (nullptr == mpFile)
	{
		return 1;
	}

	mbInit = true;
	return 0;
}

int CLogmanger::StopLog()
{
	mbInit = false;

	return 0;
}

int CLogmanger::PrintItem(tagLogItem& stLogItem)
{
#if (defined PLATFORM_WINDOWS)
	if (miType == LOG_TYPE_SCREEN && stLogItem.iLevel == LOG_LEVEL_ERR)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
#endif

	fprintf(mpFile, "%s\n", stLogItem.strLog.c_str());
	if (miType == LOG_TYPE_TEE)
	{
		fprintf(stdout, "%s\n", stLogItem.strLog.c_str());
	}

#if (defined PLATFORM_WINDOWS)
	if (miType == LOG_TYPE_SCREEN && stLogItem.iLevel == LOG_LEVEL_ERR)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
	}
#endif

	atomic_change((long*)&mlCount, (int)stLogItem.strLog.size());
	return Check();
}