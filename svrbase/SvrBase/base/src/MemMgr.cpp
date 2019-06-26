#include "MemMgr.h"
#include "util.h"
#define MEM_MGR_ALLOC_MIN_LIMIT	1024
#define MEM_MGR_ALLOC_ALIGN	1024
#define MEM_MGR_ALLOC_CHECK_RAND	0xFFFFF
#define MEM_MGR_ALLOC_HEAD_LEN (sizeof(tagMemMgrHead))

CMemMgr::CMemMgr(){
	miTotolMem = 0;
	miTotalMalloc = 0;
	miTotalFree = 0;
	miMemItemsNum = 0;
	miMapMemNums = 0;
	miAlign = MEM_MGR_ALLOC_ALIGN;
	miAllocMinLimit = MEM_MGR_ALLOC_MIN_LIMIT;
	miCheckFlag = rand() % MEM_MGR_ALLOC_CHECK_RAND;
	mpTotalMemMutex = new CUvMutex(true);
	mpMapMemMutex = new CUvMutex(true);
}

CMemMgr::~CMemMgr(){
	DODELETE(mpTotalMemMutex);
	DODELETE(mpMapMemMutex);
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
		uint32_t iIndex = (uint32_t)(iNeedLen / miAlign);
		std::queue<void*>* pQueTmp = NULL;
		CMutex* pMutex = NULL;
		mpMapMemMutex->Lock();
		std::map<uint32_t, tagMemMgrItem>::iterator iter = mmapMemItems.find(iIndex);
		if (iter != mmapMemItems.end()) {
			pQueTmp = iter->second.pQueMemItems;
			pMutex = iter->second.pMutex;
		}
		mpMapMemMutex->UnLock();

		if (pQueTmp && pMutex) {
			pMutex->Lock();
			if (!pQueTmp->empty()) {
				pRet = pQueTmp->front();
				pQueTmp->pop();
			}
			pMutex->UnLock();
		} else {
			mpMapMemMutex->Lock();
			pQueTmp = new std::queue<void *>();
			pMutex = new CUvMutex(true);
			tagMemMgrItem stMemMgrItem;
			stMemMgrItem.pQueMemItems = pQueTmp;
			stMemMgrItem.pMutex = pMutex;
			mmapMemItems.insert(std::make_pair(iIndex, stMemMgrItem));
			miMapMemNums++;
			mpMapMemMutex->UnLock();
		}
	} else {
		iNeedLen = miAllocMinLimit;
	}

	if (!pRet) {
		while (!pRet) {
			pRet = malloc(iNeedLen);
		}

		mpTotalMemMutex->Lock();
		miTotolMem += iNeedLen;
		miTotalMalloc += iLen;
		miMemItemsNum++;
		mpTotalMemMutex->UnLock();
	} else {
		mpTotalMemMutex->Lock();
		miTotalMalloc += iLen;
		mpTotalMemMutex->UnLock();
	}

	if (pRet) {
		tagMemMgrHead* pMemHead = (tagMemMgrHead*)pRet;
		memset(pRet, 0, iNeedLen);
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
		size_t iUseLen = pMemHead->iUse;
		void* pRealData = (void*)pMemHead;
		//printf("Mem Free :%lld\n", iRealLen);
		if (iRealLen == miAllocMinLimit) {
			free(pRealData);
			mpTotalMemMutex->Lock();
			miTotolMem -= iRealLen;
			miMemItemsNum--;
			miTotalFree += iUseLen;
			mpTotalMemMutex->UnLock();
		} else if (iRealLen % miAlign == 0) {
			std::queue<void*>* pQueTmp = NULL;
			CMutex* pMutex = NULL;
			uint32_t iIndex = (uint32_t)(iRealLen / miAlign);
			mpMapMemMutex->Lock();
			std::map<uint32_t, tagMemMgrItem>::iterator iter = mmapMemItems.find(iIndex);
			if (iter != mmapMemItems.end()) {
				pQueTmp = iter->second.pQueMemItems;
				pMutex = iter->second.pMutex;
			}
			mpMapMemMutex->UnLock();

			if (pQueTmp && pMutex) {
				pMutex->Lock();
				pQueTmp->push(pRealData);
				pMutex->UnLock();

				mpTotalMemMutex->Lock();
				miTotalFree += iUseLen;
				mpTotalMemMutex->UnLock();
			} else {
				printf("Error Mem Index :%d\n", iIndex);
			}
		} else {
			printf("Error Mem Free :%ld\n", (unsigned long)iRealLen);
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

	return pRet;
}

void* CMemMgr::MemCalloc(size_t count, size_t size) {
	return sMemMgr->DoCalloc(count, size);
}

void* CMemMgr::DoRealloc(void* ptr, size_t size) {
	void* pRet = NULL;
	pRet = MemMalloc(size);
	tagMemMgrHead* pTmp = (tagMemMgrHead*)ptr;
	pTmp--;
	if (pTmp) {
		size_t iRealLen = pTmp->iUse > size ? size : pTmp->iUse;
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