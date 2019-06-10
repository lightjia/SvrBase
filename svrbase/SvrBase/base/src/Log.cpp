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
	mlCurFileCount = 0;
    miCurrentLogItemNum = 0;
	miTotalCount = 0;
	mbInit = false;
	mpFile = stdout;
}

CLog::~CLog(){
	if (NULL != mpFile){
		fclose(mpFile);
		mpFile = NULL;
	}
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
		if (mlCurFileCount >= MAX_PER_LOGFILE_SIZE){
				mlCurFileCount = 0;
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
    char* pLog = (char*)MemMalloc(sizeof(char) * (lBufferSize + 1));
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
            char* pTmpLog = (char*)MemMalloc(sizeof(char) * (lBufferSize + 1));
            memcpy(pTmpLog, pLog, nPos);
            MemFree(pLog);
            pLog = pTmpLog;
        }
    }

    pLog[nPos] = '\n';
    nPos++;
    if (mstLogParam.pLogCb) {
		mstLogParam.pLogCb(iLevel, pLog);
    }

	miTotalCount += nPos;
    bool bNewLog = true;
    mcQueLogItemsMutex.Lock();
    for (;;) {
        std::vector<tagLogItem*>* pVecFreeLogItems = NULL;
        std::map<int, std::vector<tagLogItem*>*>::iterator iter = mMapFreeLogItems.find(iLevel);
        if (iter == mMapFreeLogItems.end()) {
            pVecFreeLogItems = new std::vector<tagLogItem*>();
            mMapFreeLogItems.insert(std::make_pair(iLevel, pVecFreeLogItems));
        } else {
            pVecFreeLogItems = iter->second;
        }

        if (!pVecFreeLogItems) {
            continue;
        }

        tagLogItem* pLogItem = NULL;
        if (!pVecFreeLogItems->empty()) {
            pLogItem = pVecFreeLogItems->back();
        } else {
            pLogItem = (tagLogItem*)MemMalloc(sizeof(tagLogItem));
            pLogItem->pLog = (char*)MemMalloc((MAX_PER_LOG_ITEM_CACHE_SIZE) * sizeof(char));
            pLogItem->iTotal = MAX_PER_LOG_ITEM_CACHE_SIZE;
			pLogItem->iLevel = iLevel;
			pLogItem->iUse = 0;
            char szLogNum[120];
            int iLogNumLen = sprintf(szLogNum, "Current Log Item Num:%d\n", ++miCurrentLogItemNum);
            if (bNewLog) {
                memcpy(pLogItem->pLog, szLogNum, iLogNumLen);
                pLogItem->iUse += iLogNumLen;
            } else {
                memcpy(pLog + nPos, szLogNum, iLogNumLen);
                nPos += iLogNumLen;
            }
            
            pVecFreeLogItems->push_back(pLogItem);
        }

        if (!pLogItem) {
            continue;
        }

        if (nPos + pLogItem->iUse < pLogItem->iTotal) {
            memcpy(pLogItem->pLog + pLogItem->iUse, pLog, nPos);
            pLogItem->iUse += nPos;

            if (mQueLogItems.empty()) {
                mQueLogItems.push(pLogItem);
                pVecFreeLogItems->pop_back();
            }

            break;
        } else {
            unsigned long iRest = pLogItem->iTotal - pLogItem->iUse;
            if (iRest > 0 && iRest < nPos) {
                bNewLog = false;
                memcpy(pLogItem->pLog + pLogItem->iUse, pLog, iRest);
                nPos -= iRest;
                memmove(pLog, pLog + iRest, nPos);
                pLogItem->iUse += iRest;
            }

            mQueLogItems.push(pLogItem);
            pVecFreeLogItems->pop_back();
        }
    }
    mcQueLogItemsMutex.UnLock();
    MemFree(pLog);

    mcCond.Signal();
}

int CLog::Init(const tagLogInitParam& stLogParam){
	if (!mbInit) {
		mstLogParam.iLogLevel = stLogParam.iLogLevel;
		mstLogParam.iLogType = stLogParam.iLogType;
		mstLogParam.strLogDir = stLogParam.strLogDir;
		mstLogParam.pLogCb = stLogParam.pLogCb;
		mstLogParam.pLogFileChangeCb = stLogParam.pLogFileChangeCb;
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

void CLog::WriteLog(tagLogItem* pLogItem, FILE* pFile) {
    ASSERT_RET(pLogItem && pLogItem->iUse > 0 && pLogItem->pLog && pFile);
   
    bool bColor = false;
    if (stdout == pFile && pLogItem->iLevel < LOG_LEVEL_DBG) {
        bColor = true;
#if (defined PLATFORM_WINDOWS)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LOG_COLOR[pLogItem->iLevel]);
#elif  (defined PLATFORM_LINUX)
        printf("%s", LOG_COLOR[pLogItem->iLevel]);
#endif
    }

    unsigned long iOff = 0;
    while (iOff < pLogItem->iUse) {
        int iWrite = (int)fwrite(pLogItem->pLog + iOff, 1, pLogItem->iUse - iOff, pFile);
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

int CLog::PrintItem(tagLogItem* pLogItem){
    ASSERT_RET_VALUE(pLogItem && mpFile, 1);

    WriteLog(pLogItem, mpFile);
    if (mstLogParam.iLogType == LOG_TYPE_TEE && mpFile != stdout){
        WriteLog(pLogItem, stdout);
    }

    mlCurFileCount += pLogItem->iUse;
    pLogItem->iUse = 0;
    memset(pLogItem->pLog, 0, pLogItem->iTotal);
	return Check();
}

int CLog::OnThreadRun() {
    std::vector<tagLogItem*> vecTmp;
    for (;;) {
#define LOG_WAIT_SEC    2 * 1000 * 1000
        mcCond.TimedWait(LOG_WAIT_SEC);
        std::queue<tagLogItem*> queTmp;
        if (!mcQueLogItemsMutex.TryLock()) {
            while (!mQueLogItems.empty()) {
                queTmp.push(mQueLogItems.front());
                mQueLogItems.pop();
            }

            for (std::vector<tagLogItem*>::iterator iter = vecTmp.begin(); iter != vecTmp.end(); ) {
                tagLogItem* pLogItem = *iter;
                if (pLogItem) {
                    std::map<int, std::vector<tagLogItem*>*>::iterator iter_map = mMapFreeLogItems.find(pLogItem->iLevel);
                    if (iter_map != mMapFreeLogItems.end()) {
                        if (!iter_map->second->empty()) {
                            tagLogItem* pTmpLogItem = iter_map->second->back();
                            if (pTmpLogItem && pTmpLogItem->iUse > 0){
                                char* pTmpLogData = pTmpLogItem->pLog;
                                pTmpLogItem->pLog = pLogItem->pLog;
                                pLogItem->pLog = pTmpLogData;
                                pLogItem->iUse = pTmpLogItem->iUse;
                                pTmpLogItem->iUse = 0;
                            }
                        }

                        iter_map->second->push_back(pLogItem);
                        iter = vecTmp.erase(iter);
                    } else {
                        fprintf(stderr, "Not find level:%d log\n", pLogItem->iLevel);
                        ++iter;
                    }
                }
            }

            if (queTmp.empty()) {
                for (std::map<int, std::vector<tagLogItem*>*>::iterator iter_map = mMapFreeLogItems.begin(); iter_map != mMapFreeLogItems.end(); ++iter_map) {
                    if (!iter_map->second->empty()) {
                        tagLogItem* pTmpLogItem = iter_map->second->back();
                        if (pTmpLogItem && pTmpLogItem->iUse > 0) {
                            queTmp.push(pTmpLogItem);
                            iter_map->second->pop_back();
                        }
                    }
                }
            }
            mcQueLogItemsMutex.UnLock();
        }
        
        while (!queTmp.empty()) {
            tagLogItem* pLogItem = queTmp.front();
            queTmp.pop();

            if (pLogItem) {
                PrintItem(pLogItem);
                vecTmp.push_back(pLogItem);
            }
        }
    }

    return 0;
}
