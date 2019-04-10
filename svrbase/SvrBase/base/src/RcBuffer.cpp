#include "RcBuffer.h"

CRcBuffer::CRcBuffer(){
	mpData = NULL;
	miDataLen = 0;
}

CRcBuffer::~CRcBuffer(){
	if (mpData) {
		MemFree(mpData);
	}
}

void CRcBuffer::Append(const void* pData, const size_t iLen, bool bNulEnd) {
	ASSERT_RET(pData && iLen > 0);
	size_t iTmpLen = miDataLen + iLen;
	if (bNulEnd) {
		iTmpLen += 1;
	}

	void* pTmpData = NULL;
	while (!pTmpData) {
		pTmpData = MemMalloc(iTmpLen);
	}

	if (mpData) {
		if (miDataLen > 0) {
			memcpy(pTmpData, mpData, miDataLen);
		}

		MemFree(mpData);
	}

	memcpy((char*)(pTmpData) + miDataLen, pData, iLen);
	miDataLen = miDataLen + iLen;
	mpData = pTmpData;

	if (bNulEnd) {
		((char*)mpData)[miDataLen] = '\0';
	}
}

void CRcBuffer::Append(CRcBuffer& cBuffer, bool bNulEnd) {
	Append(cBuffer.GetData(), cBuffer.GetDataLen(), bNulEnd);
}