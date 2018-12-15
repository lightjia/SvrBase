#ifndef __CCOREMGR__H_
#define __CCOREMGR__H_
#include "singleton.h"
#include "CLogmanager.h"

#if (defined PLATFORM_WINDOWS)
#include<Windows.h>  
#include<DbgHelp.h>  
#pragma comment(lib,"DbgHelp.lib") 
#elif  (defined PLATFORM_LINUX)
#endif

class CCoreMgr : public CSingleton<CCoreMgr>{
    SINGLE_CLASS_INITIAL(CCoreMgr);
public:
    ~CCoreMgr();

#if (defined PLATFORM_WINDOWS)
    static void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
    static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);
#endif

public:
    int Init();
    int UnInit();

private:
    bool mbInit;
};

#define sCCoreMgr CCoreMgr::Instance()
#endif