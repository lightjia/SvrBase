#include "MemOper.h"

CMemOper::CMemOper(mem_malloc_func pMalloc, mem_free_func pFree, mem_calloc_func pCalloc, mem_realloc_func pRealloc){
	SetMemOperFunc(pMalloc, pFree, pCalloc, pRealloc);
}

CMemOper::~CMemOper(){
}

void CMemOper::SetMemOperFunc(mem_malloc_func pMalloc, mem_free_func pFree, mem_calloc_func pCalloc, mem_realloc_func pRealloc) {
	mpMalloc = pMalloc;
	mpFree = pFree;
	mpCalloc = pCalloc;
	mpRealloc = pRealloc;
}

void* CMemOper::MemMalloc(size_t iLen) {
	void* pRet = NULL;
	if (mpMalloc && iLen > 0) {
		pRet = mpMalloc(iLen);
	}

	return pRet;
}

void CMemOper::MemFree(void* pData) {
	if (mpFree && pData) {
		mpFree(pData);
		pData = NULL;
	}
}

void CMemOper::MemFreep(void** pData) {
	if (pData) {
		MemFree(*pData);
		*pData = NULL;
	}
}

void* CMemOper::MemCalloc(size_t count, size_t size) {
	void* pRet = NULL;
	if (mpCalloc && count > 0 && size > 0) {
		pRet = mpCalloc(count, size);
	}

	return pRet;
}

void* CMemOper::MemRealloc(void* ptr, size_t size) {

}