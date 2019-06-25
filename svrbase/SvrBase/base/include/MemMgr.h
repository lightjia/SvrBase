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

struct tagMemMgrItem {
	std::queue<void*>* pQueMemItems;
	CMutex* pMutex;
};
#pragma pack()

class CMemMgr : public CSingleton<CMemMgr>{
	SINGLE_CLASS_INITIAL(CMemMgr);
public:
	~CMemMgr();

public:
	const uint64_t GetTotalMem() { return miTotolMem; }
	const uint64_t GetTotalMalloc() { return miTotalMalloc; }
	const uint64_t GetTotalFree() { return miTotalFree; }
	const uint32_t GetMemItemsNum() { return miMemItemsNum; }
	const uint32_t GetMapMemNums() { return miMapMemNums; }
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
	uint32_t miMemItemsNum;
	uint32_t miMapMemNums;
	uint64_t miTotolMem;
	CMutex* mpTotalMemMutex;
	std::map<uint32_t, tagMemMgrItem> mmapMemItems;
	CMutex* mpMapMemMutex;
	uint64_t miTotalMalloc;
	uint64_t miTotalFree;
};

#define sMemMgr CMemMgr::Instance()

const struct mem_oper_func MEMMGR_MEM_FUNC = {CMemMgr::MemMalloc, CMemMgr::MemFree, CMemMgr::MemCalloc, CMemMgr::MemRealloc };

class CClassMemMgr {
public:
	static void * operator new(size_t size);
	static void operator delete(void * p);
};
#endif