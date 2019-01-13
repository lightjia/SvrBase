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
	mpTmpFile = NULL;
    mpLogCb = NULL;
}

CLogmanger::~CLogmanger()
{
	if (NULL != mpFile)
	{
		fclose(mpFile);
		mpFile = NULL;
	}

	if (NULL != mpTmpFile)
	{
		fclose(mpTmpFile);
		mpTmpFile = NULL;
	}
}

FILE* CLogmanger::GetFile()
{
	if (mpTmpFile != NULL  && mpTmpFile != stdout)
	{
		fclose(mpTmpFile);
		mpTmpFile = NULL;
	}

	mpTmpFile = mpFile;
    FILE* pTmpTile = stdout;
	if (miType == LOG_TYPE_FILE || miType == LOG_TYPE_TEE)
	{
		struct systemtime_t stNow = get_now_time();
		char szFileName[1000];
		snprintf(szFileName, 1000, "%02d-%02d-%02d-%02d-%02d-%02d.log", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec);
		std::string strTmp = mstrDir;
		if (!str_end_with(strTmp, "\\") && !str_end_with(strTmp, "/"))
		{
			strTmp += "/";
		}

		strTmp += szFileName;
		pTmpTile = safe_open_file(strTmp.c_str(), "a+");
		if (NULL == pTmpTile)
		{
			fprintf(stderr, "open  log file err:%s", strTmp.c_str());
            pTmpTile = stdout;
		}
	}
	
    mpFile = pTmpTile;
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
	if (!mbInit || NULL == pPath || str_cmp(pPath, mstrDir.c_str(), true))
	{
		return 1;
	}

	mstrDir = pPath;
	GetFile();
	return 0;
}

void CLogmanger::AddLogItem(int iLevel, const char *format, ...)
{
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
		nPos = snprintf(szBuf, MAX_PER_LINE_LOG, "[%02d/%02d/%02d %02d:%02d:%02d.%06d] ", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec, stNow.tmmilliseconds);
	}
	
	va_list ap;
	va_start(ap, format);
#if (defined PLATFORM_WINDOWS)
	nPos += vsnprintf_s(szBuf + nPos, MAX_PER_LINE_LOG - nPos, _TRUNCATE, format, ap);
#elif  (defined PLATFORM_LINUX)
	nPos += vsnprintf(szBuf + nPos, MAX_PER_LINE_LOG - nPos, format, ap);
#endif
	va_end(ap);

    if (mpLogCb) {
        mpLogCb(iLevel, szBuf);
    }

	tagLogItem stLog;
	stLog.iLevel = iLevel;
	stLog.strLog = szBuf;
	PrintItem(stLog);
}

int CLogmanger::Init(int iType, int iLevel, const char* szDir, log_cb pLogCb)
{
  if (NULL != pLogCb)
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
	if (NULL != szDir && strlen(szDir) > 0 && !str_cmp(szDir, ".", true))
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

	if (NULL == mpFile)
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