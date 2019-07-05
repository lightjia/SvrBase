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
    void SetKey(const char* pKey, unsigned int iLen);
	void SetAligin(int iAlign);
	CMemBuffer* DesEncode(const char* psrc, size_t iLen);
	CMemBuffer* DesDecode(const char* psrc, size_t iLen);

private:
    CMemBuffer mcKey;
	int miAlign;
};
#endif
