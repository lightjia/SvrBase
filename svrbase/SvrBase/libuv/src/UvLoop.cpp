#include "UvLoop.h"

#define MAX_UV_LOOP_BAK_FD  10000

CUvLoop::CUvLoop(){
    if (uv_loop_init(&mstUvLoop) == 0) {
        SetUvLoop(&mstUvLoop);
    }

    miBakFdNum = 0;
}

CUvLoop::~CUvLoop(){
    if (NULL != mpUvLoop) {
        uv_loop_close(mpUvLoop);
        mpUvLoop = NULL;
    }
}

int CUvLoop::OnInit() {
    uv_handle_set_data((uv_handle_t*)&mstUvAsync, (void*)this);
    uv_async_init(mpUvLoop, &mstUvAsync, CUvLoop::AsyncCb);

    return Start();
}

void CUvLoop::AsyncCb(uv_async_t* pHandle) {
    CUvLoop* pUvLoop = (CUvLoop*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pUvLoop) {
        pUvLoop->PopUvBase();
    }
}

void CUvLoop::PopUvBase() {
    CUvBase* pUvBase = NULL;
   
    mcQueBaseMutex.Lock();
    if (!mqueUvBase.empty()) {
        pUvBase = mqueUvBase.front();
        mqueUvBase.pop();
    }
    mcQueBaseMutex.UnLock();

    if (NULL != pUvBase) {
        pUvBase->SetUvLoop(mpUvLoop);
        pUvBase->Init();
    }else {
        return;
    }

    miBakFdNum = uv_backend_fd(mpUvLoop);
    LOG_INFO("miBakFdNum = %d", miBakFdNum);
    PopUvBase();
}

int CUvLoop::PushUvBase(CUvBase* pUvBase) {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != pUvBase && miBakFdNum <= MAX_UV_LOOP_BAK_FD, 1);
    mcQueBaseMutex.Lock();
    mqueUvBase.push(pUvBase);
    mcQueBaseMutex.UnLock();

    return uv_async_send(&mstUvAsync);
}

int CUvLoop::OnThreadRun() {
    ASSERT_RET_VALUE(NULL != mpUvLoop, 1);
    
    //always run 
    return uv_run(mpUvLoop, UV_RUN_DEFAULT);
}