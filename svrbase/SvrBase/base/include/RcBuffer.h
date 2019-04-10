#ifndef __CRCBUFFER__H_
#define __CRCBUFFER__H_
#include "RcObject.h"

class CRcBuffer : public CRcObject, public CMemOper{
public:
	CRcBuffer();
	~CRcBuffer();

public:
	void* GetData() { return mpData; }
	size_t GetDataLen() { return miDataLen; }
	void Append(const void* pData, const size_t iLen, bool bNulEnd = false);
	void Append(CRcBuffer& cBuffer, bool bNulEnd = false);

private:
	void* mpData;
	size_t miDataLen;
};

#endif