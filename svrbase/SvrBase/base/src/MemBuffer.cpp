#include "MemBuffer.h"

CMemBuffer::CMemBuffer(size_t iLen):CMemOper(MEMMGR_MEM_FUNC){
	if (iLen < MEM_BUFFER_DEFAULT_LEN) {
		iLen = MEM_BUFFER_DEFAULT_LEN;
	}

	mpBuffer = NULL;
	miBufferLen = iLen;
	miBufferUseLen = 0;
}

CMemBuffer::~CMemBuffer(){
	MemFree(mpBuffer);
	miBufferLen = 0;
	miBufferUseLen = 0;
}

void CMemBuffer::AppendNul() {
	if (mpBuffer) {
		char* pTmp = (char*)mpBuffer;
		pTmp[miBufferUseLen] = '\0';
	}
}

void CMemBuffer::Append(void* pData, size_t iLen) {
	if (pData && iLen > 0) {
		if (!mpBuffer) {
			if (iLen > miBufferLen) {
				miBufferLen += iLen - 1;
			}

			mpBuffer = MemMalloc(miBufferLen);
		}

		if (iLen + miBufferUseLen < miBufferLen) {
			memcpy((char*)mpBuffer + miBufferUseLen, pData, iLen);
			miBufferUseLen += iLen;
		} else {
			mpBuffer = MemRealloc(mpBuffer, miBufferLen + iLen + 1);
			miBufferLen += iLen;
			memcpy((char*)mpBuffer + miBufferUseLen, pData, iLen);
			miBufferUseLen += iLen;
		}
	} else {
		fprintf(stderr, "Error CMemBuffer::Append");
	}
}