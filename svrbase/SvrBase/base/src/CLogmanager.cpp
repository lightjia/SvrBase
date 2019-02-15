#include "CLogmanager.h"

#ifdef WIN32
const static WORD LOG_COLOR[LOG_LEVEL_MAX] = {
    FOREGROUND_RED | FOREGROUND_INTENSITY,
    0,
    0,
    0,
    0
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

CLogmanger::CLogmanger(){
	miLevel = LOG_LEVEL_ERR;
	miType = LOG_TYPE_SCREEN;
	mlCount = 0;
	mstrDir = "";
	mbInit = false;
	mpFile = stdout;
    mpLogCb = NULL;
}

CLogmanger::~CLogmanger(){
	if (NULL != mpFile){
		fclose(mpFile);
		mpFile = NULL;
	}
}

FILE* CLogmanger::GetFile(){
    FILE* pTmpTile = stdout;
	if (miType == LOG_TYPE_FILE || miType == LOG_TYPE_TEE){
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
	
    SetLogFile(pTmpTile);

	return mpFile;
}

int CLogmanger::Check()
{
	//文件过大创建新文件
	if (miType == LOG_TYPE_FILE || miType == LOG_TYPE_TEE){
		if (mlCount >= MAX_PER_LOGFILE_SIZE){
				mlCount = 0;
				GetFile();
		}
	}

	return 0;
}

int CLogmanger::SetLogType(int iType){
	if (iType < LOG_TYPE_SCREEN || iType >= LOG_TYPE_MAX){
		return -1;
	}

    mcConfMutex.Lock();
	miType = iType;
    mcConfMutex.UnLock();

	GetFile();

	return 0;
}

int CLogmanger::SetLogLevel(int iLevel){
	if (iLevel < LOG_LEVEL_ERR){
		return -1;
	}

    mcConfMutex.Lock();
	miLevel = iLevel;
    mcConfMutex.UnLock();

	return 0;
}

int CLogmanger::SetLogPath(const char* pPath){
	if (NULL == pPath || str_cmp(pPath, mstrDir.c_str(), true)){
		return 1;
	}

    mcConfMutex.Lock();
	mstrDir = pPath;
    mcConfMutex.UnLock();

    if (make_dirs(mstrDir.c_str())){
        fprintf(stderr, "make_dirs error!");
    }

	GetFile();
	return 0;
}

int CLogmanger::SetLogFile(FILE* pFile) {
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

void CLogmanger::AddLogItem(int iLevel, const char *format, ...){
	if (iLevel > miLevel || iLevel > LOG_LEVEL_MAX || !mbInit){
		return;
	}

	char szBuf[MAX_PER_LINE_LOG + 1];
	memset(szBuf, 0, sizeof(szBuf));
	int nPos = 0;
	if (iLevel <= LOG_LEVEL_INFO){
		struct systemtime_t stNow = get_now_time();
		nPos = snprintf(szBuf, MAX_PER_LINE_LOG, "[%02d/%02d/%02d %02d:%02d:%02d.%03d] ", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec, stNow.tmmilliseconds);
	}
	
	va_list ap;
	va_start(ap, format);
#if (defined PLATFORM_WINDOWS)
	nPos += vsnprintf_s(szBuf + nPos, MAX_PER_LINE_LOG - nPos, _TRUNCATE, format, ap);
#elif  (defined PLATFORM_LINUX)
	nPos += vsnprintf(szBuf + nPos, MAX_PER_LINE_LOG - nPos, format, ap);
#endif
	va_end(ap);

    szBuf[nPos] = '\n';
    nPos++;
    if (mpLogCb) {
        mpLogCb(iLevel, szBuf);
    }
	
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
            pLogItem = (tagLogItem*)do_malloc(sizeof(tagLogItem));
            pLogItem->pLog = (char*)do_malloc(MAX_PER_LOG_ITEM_CACHE_SIZE * sizeof(char));
            pLogItem->iTotal = MAX_PER_LOG_ITEM_CACHE_SIZE;
            pLogItem->iLevel = iLevel;
            pVecFreeLogItems->push_back(pLogItem);
        }

        if (!pLogItem) {
            continue;
        }

        if (nPos + pLogItem->iUse < pLogItem->iTotal) {
            memcpy(pLogItem->pLog + pLogItem->iUse, szBuf, nPos);
            pLogItem->iUse += nPos;

            if (mQueLogItems.empty()) {
                mQueLogItems.push(pLogItem);
                pVecFreeLogItems->pop_back();
            }

            break;
        } else {
            mQueLogItems.push(pLogItem);
            pVecFreeLogItems->pop_back();
        }
    }
    mcQueLogItemsMutex.UnLock();

    mcCond.Signal();
}

int CLogmanger::Init(int iType, int iLevel, const char* szDir, log_cb pLogCb){
  if (NULL != pLogCb){
    mpLogCb = pLogCb;
    mbInit = true;
    return 0;
  }

	if (mbInit){
		return 1;
	}

    SetLogType(iType);
    SetLogLevel(iLevel);

    std::string strLogDir;
	if (NULL != szDir && strlen(szDir) > 0 && !str_cmp(szDir, ".", true)){
        strLogDir = szDir;
	} else {
        strLogDir = get_app_path();
        strLogDir += "log";
	}

    SetLogPath(strLogDir.c_str());
    Start();
	mbInit = true;
	return 0;
}

int CLogmanger::StopLog()
{
	mbInit = false;

	return 0;
}

void CLogmanger::WriteLog(tagLogItem* pLogItem, FILE* pFile) {
    ASSERT_RET(pLogItem && pLogItem->iUse > 0 && pLogItem->pLog && pFile);
    unsigned int iOff = 0;
    bool bColor = false;
    if (stdout == pFile && pLogItem->iLevel < LOG_LEVEL_DBG) {
        bColor = true;
#if (defined PLATFORM_WINDOWS)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LOG_COLOR[pLogItem->iLevel]);
#elif  (defined PLATFORM_LINUX)
        printf("%s", LOG_COLOR[pLogItem->iLevel]);
#endif
    }
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

int CLogmanger::PrintItem(tagLogItem* pLogItem)
{
    ASSERT_RET_VALUE(pLogItem && mpFile, 1);

    WriteLog(pLogItem, mpFile);
    if (miType == LOG_TYPE_TEE && mpFile != stdout)
    {
        WriteLog(pLogItem, stdout);
    }

    mlCount += pLogItem->iUse;
    pLogItem->iUse = 0;
    memset(pLogItem->pLog, 0, pLogItem->iTotal);
	return Check();
}

int CLogmanger::OnThreadRun() {
    std::vector<tagLogItem*> vecTmp;
    for (;;) {
        mcCond.Wait();
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
                        iter_map->second->push_back(pLogItem);
                        iter = vecTmp.erase(iter);
                    } else {
                        fprintf(stderr, "Not find level:%d log\n", pLogItem->iLevel);
                        ++iter;
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