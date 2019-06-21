#ifndef __CMEMBUFFER__H_
#define __CMEMBUFFER__H_
#include "RcObject.h"
#include "MemMgr.h"

class CMemBuffer : public CRcObject, public CMemOper, public CClassMemMgr{
public:
	CMemBuffer();
	~CMemBuffer();
	CMemBuffer(const CMemBuffer& cMemBuffer);
	CMemBuffer& operator =(const CMemBuffer& cMemBuffer);

public:
	void Append(const void* pData, const size_t iLen);
	void AppendNul();
	void* GetBuffer(size_t iIndex = 0);
	const size_t GetBuffLen() { return miBufferUseLen; }
	void SetBuffLen(const size_t iLen);
	void* AllocBuffer(size_t iLen);

private:
	int SetBuffer(const void* pData, const size_t iDataLen);

private:
	void* mpBuffer;
	int miAppendNum;
	size_t miBufferLen;
	size_t miBufferUseLen;
};
#endif