#include "MemBuffer.h"
#define MEM_BUFFER_DEFAULT_LEN 1024

CMemBuffer::CMemBuffer():CMemOper(MEMMGR_MEM_FUNC){
	mpBuffer = NULL;
	miBufferLen = 0;
	miBufferUseLen = 0;
	miAppendNum = 0;
}

CMemBuffer::~CMemBuffer(){
	MemFree(mpBuffer);
	miBufferLen = 0;
	miBufferUseLen = 0;
}

CMemBuffer::CMemBuffer(const CMemBuffer& cMemBuffer) {
	if (this != &cMemBuffer) {
		SetBuffer(cMemBuffer.mpBuffer, cMemBuffer.miBufferUseLen);
	}
}

CMemBuffer& CMemBuffer::operator =(const CMemBuffer& cMemBuffer) {
	if (this != &cMemBuffer) {
		SetBuffer(cMemBuffer.mpBuffer, cMemBuffer.miBufferUseLen);
	}

	return *this;
}

int CMemBuffer::SetBuffer(const void* pData, const size_t iDataLen) {
	if (!pData || iDataLen <= 0) {
		return 1;
	}

	MemFree(mpBuffer);
	mpBuffer = NULL;
	miBufferUseLen = 0;
	miBufferLen = 0;
	miAppendNum = 0;
	Append(pData, iDataLen);
	return 0;
}

void* CMemBuffer::GetBuffer(size_t iIndex) {
	if (!mpBuffer || iIndex > miBufferUseLen) {
		return NULL;
	}

	char* pTmp = (char*)mpBuffer;
	pTmp += iIndex;
	return (void*)pTmp;
}

void CMemBuffer::SetBuffLen(const size_t iLen) {
	if (iLen > miBufferLen) {
		fprintf(stderr, "Mem Over Limit");
		return;
	}

	miBufferUseLen = iLen;
}

void* CMemBuffer::AllocBuffer(size_t iLen) {
	if (iLen <= 0) {
		return NULL;
	}

	if (!mpBuffer) {
		if (iLen < MEM_BUFFER_DEFAULT_LEN) {
			miBufferLen = MEM_BUFFER_DEFAULT_LEN;
		} else {
			miBufferLen = iLen;
		}

		mpBuffer = MemMalloc(miBufferLen + 1);
	} else {
		if (iLen > miBufferLen) {
			mpBuffer = MemRealloc(mpBuffer, miBufferLen + iLen + 1);
			miBufferLen += iLen;
		}
	}

	return mpBuffer;
}

void CMemBuffer::AppendNul() {
	if (mpBuffer) {
		char* pTmp = (char*)mpBuffer;
		pTmp[miBufferUseLen] = '\0';
	}
}

void CMemBuffer::Append(const void* pData, const size_t iLen) {
	if (pData && iLen > 0) {
		if (!mpBuffer) {
			if (iLen < MEM_BUFFER_DEFAULT_LEN) {
				miBufferLen = MEM_BUFFER_DEFAULT_LEN;
			} else {
				miBufferLen = iLen;
			}

			mpBuffer = MemMalloc(miBufferLen + 1);
		}

		if (iLen + miBufferUseLen > miBufferLen) {
			miBufferLen += MEM_BUFFER_DEFAULT_LEN * (++miAppendNum);
			mpBuffer = MemRealloc(mpBuffer, miBufferLen + iLen + 1);
			miBufferLen += iLen;
		} 

		memcpy((char*)mpBuffer + miBufferUseLen, pData, iLen);
		miBufferUseLen += iLen;
	} else {
		fprintf(stderr, "Error CMemBuffer::Append");
	}
}