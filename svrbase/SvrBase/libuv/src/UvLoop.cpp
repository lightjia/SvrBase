#include "UvLoop.h"
#include "Log.h"

CUvLoop::CUvLoop(uv_run_mode iRunMode, uv_loop_t* pUvLoop){
    miBakFdNum = 0;
	miUvRunMode = UV_RUN_DEFAULT;
	if (iRunMode >= UV_RUN_DEFAULT && iRunMode <= UV_RUN_NOWAIT) {
		miUvRunMode = iRunMode;
	}

	if (pUvLoop) {
		mpUvLoop = pUvLoop;
	} else {
		if (!uv_loop_init(&mstUvLoop)) {
			mpUvLoop = &mstUvLoop;
		}
	}
}

CUvLoop::~CUvLoop(){
    if (mpUvLoop) {
        uv_loop_close(mpUvLoop);
        mpUvLoop = NULL;
    }
}

int CUvLoop::StartLoop() {
	ASSERT_RET_VALUE(mpUvLoop, 1);
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
	std::queue<tagUvLoopCb> queTmp;
	mcUvLoopCbsMutex.Lock();
	while (!mqueUvLoopCbs.empty()) {
		tagUvLoopCb stTmp = mqueUvLoopCbs.front();
		mqueUvLoopCbs.pop();
		queTmp.push(stTmp);
	}
	mcUvLoopCbsMutex.UnLock();

	while (!queTmp.empty()) {
		tagUvLoopCb stTmp = queTmp.front();
		queTmp.pop();
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
    ASSERT_RET_VALUE(mpUvLoop, 1);
    
    //always run 
    return uv_run(mpUvLoop, miUvRunMode);
}