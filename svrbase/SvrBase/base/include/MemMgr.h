#ifndef __CMEMMGR__H_
#define __CMEMMGR__H_
#include "singleton.h"
#include <queue>
#include <vector>
#include "UvMutex.h"
#include "MemOper.h"

#pragma pack(1)
struct tagMemMgrHead {
	int iCheckFlag;
	size_t iTotal;
	size_t iUse;
};
#pragma pack()

class CMemMgr : public CSingleton<CMemMgr>{
	SINGLE_CLASS_INITIAL(CMemMgr);
public:
	~CMemMgr();

public:
	const size_t GetTotalMem() { return miTotolMem; }
	const size_t GetTotalUseMem() { return miTotalUseMem; }
	void SetAlign(unsigned int iAlign);
	void SetAllocMinLimit(unsigned int iMinLimit);

public:
	static void* MemMalloc(size_t iLen);
	static void MemFree(void* pData);
	static void* MemCalloc(size_t count, size_t size);
	static void* MemRealloc(void* ptr, size_t size);

private:
	void* DoMalloc(size_t iLen);
	void  DoFree(void* pData);
	void* DoCalloc(size_t count, size_t size);
	void* DoRealloc(void* ptr, size_t size);

private:
	int miCheckFlag;
	unsigned int miAlign;
	unsigned int miAllocMinLimit;
	size_t miTotolMem;
	size_t miTotalUseMem;
	std::vector<std::queue<void*>*> mvecMem;
	std::vector<CMutex*> mvecMemItemLock;
	CMutex* mpVecMemMutex;
	CMutex* mpTotalMemMutex;
};

#define sMemMgr CMemMgr::Instance()

const struct mem_oper_func MEMMGR_MEM_FUNC = {CMemMgr::MemMalloc, CMemMgr::MemFree, CMemMgr::MemCalloc, CMemMgr::MemRealloc };

class CClassMemMgr {
public:
	static void * operator new(size_t size);
	static void operator delete(void * p);
};
#endif