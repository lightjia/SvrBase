#ifndef __COPENSSLRSA__H_
#define __COPENSSLRSA__H_
#include "opensslinc.h"
#include "Log.h"
class COpensslRsa{
public:
    COpensslRsa();
    ~COpensslRsa();

public:
    void SetPubKey(const char* pKey, unsigned int iLen);
    void SetPriKey(const char* pKey, unsigned int iLen);
	CMemBuffer* RsaEncode(const char* psrc, size_t iLen);
	CMemBuffer* RsaDecode(const char* psrc, size_t iLen);

private:
    CMemBuffer mcPubKey;
	CMemBuffer mcPriKey;
};
#endif
