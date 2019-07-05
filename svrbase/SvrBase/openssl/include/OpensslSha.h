#ifndef __COPENSSLSHA__H_
#define __COPENSSLSHA__H_
#include "singleton.h"
#include "opensslinc.h"
#include "Log.h"
class COpensslSha : public CSingleton<COpensslSha>{
    SINGLE_CLASS_INITIAL(COpensslSha);
public:
    ~COpensslSha();

public:
    std::string Sha1(const unsigned char* psrc, int iSrcLen);
	std::string Sha224(const unsigned char* psrc, int iSrcLen);
	std::string Sha256(const unsigned char* psrc, int iSrcLen);
	std::string Sha384(const unsigned char* psrc, int iSrcLen);
	std::string Sha512(const unsigned char* psrc, int iSrcLen);
};

#define sOpensslSha COpensslSha::Instance()
#endif