#ifndef __COPENSSLRSA__H_
#define __COPENSSLRSA__H_
#include "opensslinc.h"
#include "Log.h"
class COpensslRsa{
public:
    COpensslRsa();
    ~COpensslRsa();

public:
    void SetPubKey(const char* pKey, int iLen);
    void SetPriKey(const char* pKey, int iLen);
    len_str RsaEncode(const char* psrc, size_t iLen);
    len_str RsaDecode(const char* psrc, size_t iLen);

private:
    len_str mlPubKey;
    len_str mlPriKey;
};
#endif
