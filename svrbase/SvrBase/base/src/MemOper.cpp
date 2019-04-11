#include "MemOper.h"

CMemOper::CMemOper(mem_oper_func stMemFunc){
	SetMemOperFunc(stMemFunc);
}

CMemOper::~CMemOper(){
}

void CMemOper::SetMemOperFunc(mem_oper_func stMemFunc) {
	mpMalloc = stMemFunc.pMallocFunc;
	mpFree = stMemFunc.pFreeFunc;
	mpCalloc = stMemFunc.pCallocFunc;
	mpRealloc = stMemFunc.pReallocFunc;
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
	void* pRet = NULL;
	if (mpRealloc && ptr && size > 0) {
		pRet = mpRealloc(ptr, size);
	}

	return pRet;
}