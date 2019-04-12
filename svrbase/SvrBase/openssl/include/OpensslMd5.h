#ifndef __COPENSSLMD5__H_
#define __COPENSSLMD5__H_
#include "singleton.h"
#include "opensslinc.h"
#include "Log.h"
class COpensslMd5 : public CSingleton<COpensslMd5>{
    SINGLE_CLASS_INITIAL(COpensslMd5);
public:
    ~COpensslMd5();

public:
    std::string Md5(const char* pSrc, int iSrcLen);
};
#define sOpensslMd5 COpensslMd5::Instance()
#endif