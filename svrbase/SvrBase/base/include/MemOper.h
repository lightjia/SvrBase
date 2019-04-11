#ifndef __CMEMOPER__H_
#define __CMEMOPER__H_
#include "common.h"

typedef void* (*mem_malloc_func)(size_t);
typedef void (*mem_free_func)(void*);
typedef void* (*mem_calloc_func)(size_t, size_t);
typedef void* (*mem_realloc_func)(void*, size_t);

struct mem_oper_func {
	mem_malloc_func  pMallocFunc;
	mem_free_func pFreeFunc;
	mem_calloc_func pCallocFunc;
	mem_realloc_func pReallocFunc;
};

const struct mem_oper_func DEFAULT_MEM_FUNC = {malloc, free, calloc, realloc};

class CMemOper{
public:
	CMemOper(mem_oper_func stMemFunc = DEFAULT_MEM_FUNC);
	virtual ~CMemOper();

public:
	void SetMemOperFunc(mem_oper_func stMemFunc);

public:
	void* MemMalloc(size_t iLen);
	void MemFree(void* pData);
	void MemFreep(void** pData);
	void* MemCalloc(size_t count, size_t size);
	void* MemRealloc(void* ptr, size_t size);

private:
	mem_malloc_func mpMalloc;
	mem_free_func mpFree;
	mem_calloc_func mpCalloc;
	mem_realloc_func mpRealloc;
};

#endif