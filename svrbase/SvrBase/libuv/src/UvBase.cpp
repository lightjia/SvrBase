#include "UvBase.h"

CUvBase::CUvBase(){
    mpUvLoop = NULL;
    BZERO(mstUvBuf);
}

CUvBase::~CUvBase(){
    if (NULL != mstUvBuf.pBuf) {
        DOFREE(mstUvBuf.pBuf);
    }
}

void CUvBase::UvBufAlloc(uv_handle_t* pStream, size_t iSize, uv_buf_t* pBuf) {
    CUvBase* pUvBase = (CUvBase*)uv_handle_get_data((uv_handle_t*)pStream);
    if (NULL != pUvBase && NULL != pUvBase->mstUvBuf.pBuf)
    {
        *pBuf = uv_buf_init(pUvBase->mstUvBuf.pBuf + pUvBase->mstUvBuf.iUse, (unsigned int)(pUvBase->mstUvBuf.iLen - pUvBase->mstUvBuf.iUse));
    }
    else
    {
        LOG_ERR("RtpStreamAlloc Error");
    }
}

int CUvBase::Init(ssize_t iBufSize) {
    ASSERT_RET_VALUE(NULL != mpUvLoop, 1);
    ASSERT_RET_VALUE(iBufSize > 0, 1);
    mstUvBuf.pBuf = (char*)do_malloc(iBufSize);
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