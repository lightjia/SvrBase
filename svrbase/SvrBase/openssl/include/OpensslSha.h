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
    len_str Sha1(const unsigned char* psrc, int iSrcLen);
    len_str Sha224(const unsigned char* psrc, int iSrcLen);
    len_str Sha256(const unsigned char* psrc, int iSrcLen);
    len_str Sha384(const unsigned char* psrc, int iSrcLen);
    len_str Sha512(const unsigned char* psrc, int iSrcLen);
};

#define sOpensslSha COpensslSha::Instance()
#endif