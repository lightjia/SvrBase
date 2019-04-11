#include "MemMgr.h"
#include "util.h"
#define MEM_MGR_ALLOC_MIN_LIMIT	1024
#define MEM_MGR_ALLOC_ALIGN	100
#define MEM_MGR_ALLOC_CHECK_RAND	0xFFFFF
#define MEM_MGR_ALLOC_HEAD_LEN (sizeof(tagMemMgrHead))

CMemMgr::CMemMgr(){
	miTotolMem = 0;
	miAlign = MEM_MGR_ALLOC_ALIGN;
	miAllocMinLimit = MEM_MGR_ALLOC_MIN_LIMIT;
	miCheckFlag = rand() % MEM_MGR_ALLOC_CHECK_RAND;
	mpTotalMemMutex = new CUvMutex(true);
	mpVecMemMutex = new CUvMutex(true);
}

CMemMgr::~CMemMgr(){
	DODELETE(mpTotalMemMutex);
	DODELETE(mpVecMemMutex);
}

void CMemMgr::SetAlign(unsigned int iAlign) {
	if (iAlign > MEM_MGR_ALLOC_ALIGN) {
		miAlign = iAlign;
	}
}

void CMemMgr::SetAllocMinLimit(unsigned int iMinLimit) {
	if (iMinLimit >= 0 && iMinLimit < MEM_MGR_ALLOC_MIN_LIMIT) {
		miAllocMinLimit = iMinLimit;
	}
}

void* CMemMgr::DoMalloc(size_t iLen) {
	void* pRet = NULL;
	if (iLen <= 0) {
		return pRet;
	}

	size_t iNeedLen = MEM_MGR_ALLOC_HEAD_LEN + iLen;
	if (iNeedLen > miAllocMinLimit) {
		iNeedLen = iNeedLen + (miAlign - iNeedLen % miAlign);
		size_t iIndex = iNeedLen / miAlign;
		std::queue<void*>* pQueTmp = NULL;
		CUvMutex* pMutex = NULL;
		mpVecMemMutex->Lock();
		if (iIndex <= mvecMem.size()) {
			pQueTmp = mvecMem[iIndex - 1];
			pMutex = mvecMemItemLock[iIndex - 1];
		} 
		mpVecMemMutex->UnLock();

		if (pQueTmp && pMutex) {
			pMutex->Lock();
			if (!pQueTmp->empty()) {
				pRet = pQueTmp->front();
				pQueTmp->pop();
			}
			pMutex->UnLock();
		} else {
			mpVecMemMutex->Lock();
			while (mvecMem.size() < iIndex) {
				std::queue<void*>* pQueTmp = new std::queue<void *>();
				mvecMem.push_back(pQueTmp);
				CUvMutex* pMutex = new CUvMutex(true);
				mvecMemItemLock.push_back(pMutex);
			}
			mpVecMemMutex->UnLock();
		}
	} else {
		iNeedLen = miAllocMinLimit;
	}

	if (!pRet) {
		pRet = malloc(iNeedLen);
		mpTotalMemMutex->Lock();
		miTotolMem += iNeedLen;
		mpTotalMemMutex->UnLock();
		//printf("Malloc Mem:%lld TotalMem:%lld\n", iNeedLen, miTotolMem);
	}

	if (pRet) {
		tagMemMgrHead* pMemHead = (tagMemMgrHead*)pRet;
		pMemHead->iTotal = iNeedLen;
		pMemHead->iUse = iLen;
		pMemHead->iCheckFlag = miCheckFlag;
		pRet = (void*)((char*)pRet + sizeof(tagMemMgrHead));
	}

	return pRet;
}

void* CMemMgr::MemMalloc(size_t iLen) {
	return sMemMgr->DoMalloc(iLen);
}

void  CMemMgr::DoFree(void* pData) {
	if (pData) {
		tagMemMgrHead* pMemHead = (tagMemMgrHead*)((char*)pData - sizeof(tagMemMgrHead));
		if (miCheckFlag != pMemHead->iCheckFlag) {
			fprintf(stderr, "Error Mem:%p\n", pData);
			return;
		}

		size_t iRealLen = pMemHead->iTotal;
		void* pRealData = (void*)pMemHead;
		//printf("Mem Free :%lld\n", iRealLen);
		if (iRealLen == miAllocMinLimit) {
			free(pRealData);
		} else if (iRealLen % miAlign == 0) {
			std::queue<void*>* pQueTmp = NULL;
			CUvMutex* pMutex = NULL;
			size_t iIndex = iRealLen / miAlign;
			mpVecMemMutex->Lock();
			if (iIndex <= mvecMem.size()) {
				pQueTmp = mvecMem[iIndex - 1];
				pMutex = mvecMemItemLock[iIndex - 1];
			}
			mpVecMemMutex->UnLock();

			if (pQueTmp && pMutex) {
				pMutex->Lock();
				pQueTmp->push(pRealData);
				pMutex->UnLock();
			} else {
				printf("Error Mem Index :%lld\n", iIndex);
			}
		} else {
			printf("Error Mem Free :%lld\n", iRealLen);
		}
	}
}

void CMemMgr::MemFree(void* pData) {
	sMemMgr->DoFree(pData);
}

void* CMemMgr::DoCalloc(size_t count, size_t size) {
	void* pRet = NULL;
	size_t iNeedLen = count * size;
	pRet = MemMalloc(iNeedLen);
	memset(pRet, 0, iNeedLen);

	return pRet;
}

void* CMemMgr::MemCalloc(size_t count, size_t size) {
	return sMemMgr->DoCalloc(count, size);
}

void* CMemMgr::DoRealloc(void* ptr, size_t size) {
	void* pRet = NULL;
	pRet = MemMalloc(size);
	size_t* pTmp = (size_t*)ptr;
	if (pTmp) {
		size_t iRealLen = pTmp[-2] > size ? size : pTmp[2];
		memcpy(pRet, ptr, iRealLen);
	}
	MemFree(ptr);
	return pRet;
}

void* CMemMgr::MemRealloc(void* ptr, size_t size) {
	return sMemMgr->DoRealloc(ptr, size);
}

void * CClassMemMgr::operator new(size_t size){
	return sMemMgr->MemMalloc(size);
}

void CClassMemMgr::operator delete(void * p) {
	sMemMgr->MemFree(p);
}