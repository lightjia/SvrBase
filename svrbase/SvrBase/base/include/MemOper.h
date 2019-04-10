#ifndef __CMEMOPER__H_
#define __CMEMOPER__H_
#include "common.h"

typedef void* (*mem_malloc_func)(size_t);
typedef void (*mem_free_func)(void*);
typedef void* (*mem_calloc_func)(size_t, size_t);
typedef void* (*mem_realloc_func)(void*, size_t);

class CMemOper{
public:
	CMemOper(mem_malloc_func pMalloc = malloc, mem_free_func pFree = free, mem_calloc_func pCalloc = calloc, mem_realloc_func pRealloc = realloc);
	virtual ~CMemOper();

public:
	void SetMemOperFunc(mem_malloc_func pMalloc, mem_free_func pFree, mem_calloc_func pCalloc, mem_realloc_func pRealloc);

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