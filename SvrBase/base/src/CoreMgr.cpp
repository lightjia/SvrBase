#include "CoreMgr.h"

CCoreMgr::CCoreMgr(){
    mbInit = false;
}

CCoreMgr::~CCoreMgr(){
}

#if (defined PLATFORM_WINDOWS)
void CCoreMgr::CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException) {
    fprintf(stderr, "Create CoreDump\n");
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Dump信息  
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;
    // 写入Dump文件内容  
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    CloseHandle(hDumpFile);
}

LONG CCoreMgr::ApplicationCrashHandler(EXCEPTION_POINTERS *pException) {
    struct systemtime_t stNow = get_now_time();
    char szFileName[1000];
    snprintf(szFileName, 1000, "%02d-%02d-%02d-%02d-%02d-%02d.dmp", stNow.tmyear, stNow.tmmon, stNow.tmmday, stNow.tmhour, stNow.tmmin, stNow.tmsec);
    CCoreMgr::CreateDumpFile(L"AnalySisSvr.dmp", pException);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int CCoreMgr::Init() {
#if (defined PLATFORM_WINDOWS)
    fprintf(stderr, "Init Core\n");
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CCoreMgr::ApplicationCrashHandler);
    mbInit = true;
#elif  (defined PLATFORM_LINUX)
#endif
    return 0;
}

int CCoreMgr::UnInit() {
    return 0;
}