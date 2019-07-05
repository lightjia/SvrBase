#ifndef __COPENSSLBASE64__H_
#define __COPENSSLBASE64__H_
#include "singleton.h"
#include "opensslinc.h"
#include "Log.h"
class COpensslBase64 : public CSingleton<COpensslBase64>{
    SINGLE_CLASS_INITIAL(COpensslBase64);
public:
    ~COpensslBase64();

public:
    CMemBuffer* Base64Encode(const char * pSrc, int iSrcLen, bool bNewLine = false);
	CMemBuffer* Base64Decode(const char * pSrc, int iSrcLen, bool bNewLine = false);
};

#define sOpensslBase64 COpensslBase64::Instance()
#endif