#ifndef __COPENSSLSHA__H_
#define __COPENSSLSHA__H_
#include "singleton.h"
#include "opensslinc.h"
#include "CLogmanager.h"
class COpensslSha : public CSingleton<COpensslSha>{
    SINGLE_CLASS_INITIAL(COpensslSha);
public:
    ~COpensslSha();

public:
    len_str Sha1(const char* psrc, int iSrcLen, char* pDst);
    len_str Sha224(const char* psrc, int iSrcLen, char* pDst);
    len_str Sha256(const char* psrc, int iSrcLen, char* pDst);
    len_str Sha384(const char* psrc, int iSrcLen, char* pDst);
    len_str Sha512(const char* psrc, int iSrcLen, char* pDst);
};

#define sOpensslSha COpensslSha::Instance()
#endif