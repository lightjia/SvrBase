#ifndef __COPENSSLDES__H_
#define __COPENSSLDES__H_
#include "opensslinc.h"
#include "Log.h"
class COpensslDes
{
public:
    COpensslDes();
    ~COpensslDes();

public:
    void SetKey(const char* pKey, int iLen);
    len_str DesEncode(const char* psrc, size_t iLen);
    len_str DesDecode(const char* psrc, size_t iLen);

private:
    len_str mlKey;
};
#endif
