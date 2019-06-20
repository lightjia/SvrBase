#ifndef __CMEMBUFFER__H_
#define __CMEMBUFFER__H_
#include "RcObject.h"
#include "MemMgr.h"
#define MEM_BUFFER_DEFAULT_LEN 1024
class CMemBuffer : public CRcObject, public CMemOper, public CClassMemMgr{
public:
	CMemBuffer(size_t iLen = MEM_BUFFER_DEFAULT_LEN);
	~CMemBuffer();

public:
	void Append(void* pData, size_t iLen);
	void AppendNul();
	void* GetBuffer(size_t iIndex = 0);
	const size_t GetBuffLen() { return miBufferUseLen; }
	void* AllocBuffer(size_t iLen);

private:
	void* mpBuffer;
	size_t miBufferLen;
	size_t miBufferUseLen;
};
#endif