#include "UvLoop.h"
#include "Log.h"

CUvLoop::CUvLoop(){
	mpUvLoop = NULL;
    miBakFdNum = 0;
}

CUvLoop::~CUvLoop(){
    if (NULL != mpUvLoop) {
        uv_loop_close(mpUvLoop);
        mpUvLoop = NULL;
    }
}

int CUvLoop::StartLoop() {
	ASSERT_RET_VALUE(!mpUvLoop && !uv_loop_init(&mstUvLoop), 1);
	mpUvLoop = &mstUvLoop;
	uv_handle_set_data((uv_handle_t*)&mstUvAsync, (void*)this);
	uv_async_init(mpUvLoop, &mstUvAsync, CUvLoop::AsyncCb);
	return Start();
}

void CUvLoop::AsyncCb(uv_async_t* pHandle) {
	CUvLoop* pUvLoop = (CUvLoop*)uv_handle_get_data((uv_handle_t*)pHandle);
	ASSERT_RET(pUvLoop);
	pUvLoop->UvCb();
}

void CUvLoop::UvCb() {
	CAutoMutex cAutoMutex(&mcUvLoopCbsMutex);
	while (!mqueUvLoopCbs.empty()) {
		tagUvLoopCb stTmp = mqueUvLoopCbs.front();
		if (stTmp.pUvLoopCb) {
			stTmp.pUvLoopCb->UvCallBack(mpUvLoop, stTmp.pCbData);
			UNREF(stTmp.pUvLoopCb);
		}
	}

    miBakFdNum = uv_backend_fd(mpUvLoop);
}

int CUvLoop::CallUv(CUvLoopCb* pUvLoopCb, void* pCbData) {
    ASSERT_RET_VALUE(mpUvLoop && pUvLoopCb, 1);
	REF(pUvLoopCb);
    mcUvLoopCbsMutex.Lock();
	tagUvLoopCb stTmp;
	stTmp.pUvLoopCb = pUvLoopCb;
	stTmp.pCbData = pCbData;
    mqueUvLoopCbs.push(stTmp);
	mcUvLoopCbsMutex.UnLock();

    return uv_async_send(&mstUvAsync);
}

int CUvLoop::OnThreadRun() {
    ASSERT_RET_VALUE(NULL != mpUvLoop, 1);
    
    //always run 
    return uv_run(mpUvLoop, UV_RUN_DEFAULT);
}