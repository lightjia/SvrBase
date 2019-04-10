#include "UvBase.h"

CUvBase::CUvBase(){
    mpUvLoop = NULL;
    BZERO(mstUvBuf);
}

CUvBase::~CUvBase() {
    MemFree(mstUvBuf.pBuf);
	mstUvBuf.pBuf = NULL;
}

void CUvBase::UvBufAlloc(uv_handle_t* pStream, size_t iSize, uv_buf_t* pBuf) {
    CUvBase* pUvBase = (CUvBase*)uv_handle_get_data((uv_handle_t*)pStream);
    ASSERT_RET(pUvBase && pUvBase->mstUvBuf.pBuf && pUvBase->mstUvBuf.iLen > 0 && pUvBase->mstUvBuf.iLen > pUvBase->mstUvBuf.iUse);
    *pBuf = uv_buf_init(pUvBase->mstUvBuf.pBuf + pUvBase->mstUvBuf.iUse, (unsigned int)(pUvBase->mstUvBuf.iLen - pUvBase->mstUvBuf.iUse));
}

int CUvBase::Init(ssize_t iBufSize) {
    ASSERT_RET_VALUE(NULL != mpUvLoop, 1);
    if (iBufSize > 0) {
        mstUvBuf.pBuf = (char*)MemMalloc(iBufSize * sizeof(char));
    }
   
    mstUvBuf.iLen = iBufSize;
    return OnInit();
}

void CUvBase::BaseTimerOut(uv_timer_t* pHandler) {
    CUvBase* pUvBase = (CUvBase*)uv_handle_get_data((uv_handle_t*)pHandler);
    if (NULL != pUvBase){
        pUvBase->OnBaseTimer();
    }
}

void CUvBase::OnBaseTimer() {
}

int CUvBase::StopBaseTimer() {
    uv_close((uv_handle_t*)&mstUvTimer, NULL);
    return 0;
}

int CUvBase::StartBaseTimer(uint64_t iTimeout, uint64_t iRepeat) {
    ASSERT_RET_VALUE(NULL != mpUvLoop, 1);
    uv_timer_init(mpUvLoop, &mstUvTimer);
    uv_handle_set_data((uv_handle_t*)&mstUvTimer, (void*)this);
    return uv_timer_start(&mstUvTimer, CUvBase::BaseTimerOut, iTimeout, iRepeat);
}