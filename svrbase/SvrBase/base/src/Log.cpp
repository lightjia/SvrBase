#include "Log.h"

#ifdef WIN32
const static WORD LOG_COLOR[LOG_LEVEL_MAX] = {
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    FOREGROUND_INTENSITY,
    FOREGROUND_INTENSITY,
    FOREGROUND_INTENSITY,
    FOREGROUND_INTENSITY
    //FOREGROUND_RED | 
};
#else

const static char LOG_COLOR[LOG_LEVEL_MAX][50] = {
    "\e[31m", //red
    "\e[0m",
    "\e[0m",
    "\e[0m",
    "\e[0m"
    //"\e[34m\e[1m",//hight blue
    //"\e[33m", //yellow
    //"\e[32m", //green
    //"\e[35m" 
};
#endif

CLog::CLog(){
	miCurFileCount = 0;
	miTotalCount = 0;
	mbInit = false;
	mpFile = stdout;
	miMaxPerLogFileSize = MAX_PER_LOGFILE_SIZE;
}

CLog::~CLog(){
	if (NULL != mpFile){
		fclose(mpFile);
		mpFile = NULL;
	}
}

int CLog::SetMaxPerLogFileSize(uint64_t iMaxPerLogFileSize) {
	if (iMaxPerLogFileSize >= miMaxPerLogFileSize) {
		miMaxPerLogFileSize = iMaxPerLogFileSize;
		return 0;
	}

	return 1;
}

FILE* CLog::GetFile(){
    FILE* pTmpTile = stdout;
	if (mstLogParam.iLogType == LOG_TYPE_FILE || mstLogParam.iLogType == LOG_TYPE_TEE){
		struct systemtime_t stNow = get_now_time();
		char szFileName[1000];
		snprintf(szFileName, 1000, "%02d-%02d-%02d-%02d-%02d-%02d.log", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec);
		std::string strTmp = mstLogParam.strLogDir;
		if (!str_end_with(strTmp, "\\") && !str_end_with(strTmp, "/")){
			strTmp += "/";
		}

		strTmp += szFileName;
		pTmpTile = safe_open_file(strTmp.c_str(), "a+");
		if (NULL == pTmpTile){
			fprintf(stderr, "open  log file err:%s\n", strTmp.c_str());
            pTmpTile = stdout;
		} else {
			if (mstLogParam.pLogFileChangeCb) {
				mstLogParam.pLogFileChangeCb(strTmp);
			}
		}
	}
	
    SetLogFile(pTmpTile);

	return mpFile;
}

int CLog::Check(){
	//文件过大创建新文件
	if (mstLogParam.iLogType == LOG_TYPE_FILE || mstLogParam.iLogType == LOG_TYPE_TEE){
		if (miCurFileCount >= miMaxPerLogFileSize){
				miCurFileCount = 0;
				GetFile();
		}
	}

	return 0;
}

int CLog::SetLogType(int iType){
	if (iType < LOG_TYPE_SCREEN || iType >= LOG_TYPE_MAX){
		fprintf(stderr, "SetLogType:%d error", iType);
		return 1;
	}

    mcConfMutex.Lock();
	mstLogParam.iLogType = (LogType)iType;
    mcConfMutex.UnLock();

	return 0;
}

int CLog::SetLogLevel(int iLevel){
	if (iLevel < LOG_LEVEL_ERR || iLevel > LOG_LEVEL_MAX){
		fprintf(stderr, "SetLogLevel:%d error", iLevel);
		return 1;
	}

    mcConfMutex.Lock();
	mstLogParam.iLogLevel = (LogLevel)iLevel;
    mcConfMutex.UnLock();

	return 0;
}

int CLog::SetLogPath(const char* pPath){
	if (LOG_TYPE_SCREEN == mstLogParam.iLogType || !pPath) {
		return 0;
	}

    mcConfMutex.Lock();
	if (make_dirs(pPath)) {
		fprintf(stderr, "make_dirs error:%s!\n", pPath);
		return 1;
	}
	mstLogParam.strLogDir = pPath;
    mcConfMutex.UnLock();

	GetFile();
	return 0;
}

int CLog::SetLogFile(FILE* pFile) {
    if (pFile) {
        mcConfMutex.Lock();
        if (mpFile && mpFile != stdout) {
            fclose(mpFile);
            mpFile = NULL;
        }

        mpFile = pFile;
        mcConfMutex.UnLock();
    }

    return 0;
}

void CLog::AddLogItem(int iLevel, const char *format, ...){
	if (iLevel > mstLogParam.iLogLevel || iLevel > LOG_LEVEL_MAX || !mbInit){
		fprintf(stderr, "UnKnow LogLevel:%d Or Not Init\n", iLevel);
		return;
	}

    unsigned long lBufferSize = MAX_PER_LINE_LOG;
	CMemBuffer* pLogBuffer = new CMemBuffer();
    char* pLog = (char*)pLogBuffer->AllocBuffer(lBufferSize);
    unsigned long nPos = 0;
    if (iLevel <= LOG_LEVEL_INFO) {
        struct systemtime_t stNow = get_now_time();
        nPos = snprintf(pLog, MAX_PER_LINE_LOG, "[%02d/%02d/%02d %02d:%02d:%02d.%03d] ", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec, stNow.tmmilliseconds);
    }

    for (;;) {
        va_list ap;
        va_start(ap, format);
        int iTmpPos = 0;
#if (defined PLATFORM_WINDOWS)
        iTmpPos = vsnprintf_s(pLog + nPos, lBufferSize - nPos, _TRUNCATE, format, ap);
#elif  (defined PLATFORM_LINUX)
        iTmpPos = vsnprintf(pLog + nPos, lBufferSize - nPos, format, ap);
#endif
        va_end(ap);

        if (iTmpPos >= 0 && (unsigned long)iTmpPos <= lBufferSize) {
            nPos += iTmpPos;
            break;
        } else {
            lBufferSize *= 2;
			pLog = (char*)pLogBuffer->AllocBuffer(lBufferSize);
        }
    }

    pLog[nPos] = '\n';
    nPos++;
	pLogBuffer->SetBuffLen(nPos);
    if (mstLogParam.pLogCb) {
		mstLogParam.pLogCb(iLevel, pLog);
    }

    mcLogMutex.Lock();
	miTotalCount += nPos;
	std::map<int, CMemBuffer*>::iterator iter = mMapLogItems.find(iLevel);
	if (iter == mMapLogItems.end()) {
		mMapLogItems.insert(std::make_pair(iLevel, pLogBuffer));
		pLogBuffer = NULL;
	} else {
		if (iter->second) {
			iter->second->Append(pLog, nPos);
		} else {
			fprintf(stderr, "insert log error");
		}
	}
    mcLogMutex.UnLock();

	DODELETE(pLogBuffer);
    mcCond.Signal();
}

int CLog::Init(const tagLogInitParam& stLogParam){
	if (!mbInit) {
		mstLogParam = stLogParam;
		SetLogType(mstLogParam.iLogType);
		SetLogLevel(mstLogParam.iLogLevel);
		if (mstLogParam.strLogDir.empty() || str_cmp(mstLogParam.strLogDir.c_str(), ".", true)) {
			mstLogParam.strLogDir = get_app_path();
			mstLogParam.strLogDir += "log";
		}

		SetLogPath(mstLogParam.strLogDir.c_str());
		Start();
		mbInit = true;
	}

	return 0;
}

int CLog::StopLog(){
	mbInit = false;

	return 0;
}

void CLog::WriteLog(CMemBuffer* pLogBuf, int iLogLevel, FILE* pFile) {   
    bool bColor = false;
    if (stdout == pFile && iLogLevel < LOG_LEVEL_DBG) {
        bColor = true;
#if (defined PLATFORM_WINDOWS)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LOG_COLOR[iLogLevel]);
#elif  (defined PLATFORM_LINUX)
        printf("%s", LOG_COLOR[iLogLevel]);
#endif
    }

    size_t iOff = 0;
	char* pLog = (char*)pLogBuf->GetBuffer();
    while (iOff < pLogBuf->GetBuffLen()) {
        int iWrite = (int)fwrite(pLog, 1, pLogBuf->GetBuffLen() - iOff, pFile);
        if (iWrite < 0) {
            fprintf(stderr, "Write Log Error\n");
            break;
        }

        iOff += iWrite;
    }

    if (bColor) {
#if (defined PLATFORM_WINDOWS)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
#elif  (defined PLATFORM_LINUX)
        printf("\e[0m");
#endif
    }
}

int CLog::PrintLog(CMemBuffer* pLogBuf, int iLogLevel){
	if (!pLogBuf) {
		return 1;
	}

    WriteLog(pLogBuf, iLogLevel, mpFile);
    if (mstLogParam.iLogType == LOG_TYPE_TEE && mpFile != stdout){
        WriteLog(pLogBuf, iLogLevel, stdout);
    }

    miCurFileCount += (unsigned long)pLogBuf->GetBuffLen();
	DODELETE(pLogBuf);

	return Check();
}

int CLog::OnThreadRun() {
    for (;;) {
#define LOG_WAIT_SEC    2 * 1000 * 1000
        mcCond.TimedWait(LOG_WAIT_SEC);
		std::map<int, CMemBuffer*> mapTmp;
		mcLogMutex.Lock();
		mapTmp = mMapLogItems;
		mMapLogItems.clear();
		mcLogMutex.UnLock();

		for (std::map<int, CMemBuffer*>::iterator iter = mapTmp.begin(); iter != mapTmp.end(); ++iter) {
			PrintLog(iter->second, iter->first);
		}
    }

    return 0;
}
