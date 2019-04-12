#include "UvTimer.h"
#include "Log.h"

CUvTimer::CUvTimer(){
    mbInit = false;
}

CUvTimer::~CUvTimer(){
}

int CUvTimer::Init() {
    if (!mbInit) {
        mbInit = true;
        return Start();
    }

    return 0;
}

int CUvTimer::UnInit() {
    if (mbInit) {
        mcMapTimerMutex.Lock();
        for (std::map<uv_timer_t*, tagUvTimerParam>::iterator iter = mmapTimer.begin(); iter != mmapTimer.end(); ++iter) {
            uv_timer_t* pTimer = iter->first;
            uv_close((uv_handle_t*)pTimer, NULL);
            MemFree(pTimer);
            mmapTimer.erase(iter++);
        }
        mcMapTimerMutex.UnLock();

        if (uv_is_active((uv_handle_t*)&mstUvAsync)) {
            uv_close((uv_handle_t*)&mstUvAsync, NULL);
        }
    }
    return 0;
}

int CUvTimer::OnThreadRun() {
    if (!uv_loop_init(&mstUvLoop)) {
        uv_handle_set_data((uv_handle_t*)&mstUvAsync, (void*)this);
        uv_async_init(&mstUvLoop, &mstUvAsync, CUvTimer::AsyncCb);
        uv_run(&mstUvLoop, UV_RUN_DEFAULT);
        uv_loop_close(&mstUvLoop);
    }
    return 0;
}

void CUvTimer::AsyncCb(uv_async_t* pHandle) {
    CUvTimer* pUvTimer = (CUvTimer*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pUvTimer) {
        pUvTimer->HandleTimer();
    }
}

void CUvTimer::TimerOut(uv_timer_t* pHandler) {
    CUvTimer* pUvTimer = (CUvTimer*)uv_handle_get_data((uv_handle_t*)pHandler);
    if (NULL != pUvTimer) {
        pUvTimer->mcMapTimerMutex.Lock();
        std::map<uv_timer_t*, tagUvTimerParam>::iterator iter = pUvTimer->mmapTimer.find(pHandler);
        if (iter != pUvTimer->mmapTimer.end()) {
            if (UV_TIMER_STATE_RUN == iter->second.iState && iter->second.pCb) {
                iter->second.pCb(iter->second.pCbParam);
            }
        } else {
            LOG_ERR("Timer Not Exist:%p", pHandler);
        }
        pUvTimer->mcMapTimerMutex.UnLock();
    }
}

void CUvTimer::HandleTimer() {
    mcMapTimerMutex.Lock();
    for (std::map<uv_timer_t*, tagUvTimerParam>::iterator iter = mmapTimer.begin(); iter != mmapTimer.end(); ++iter) {
        uv_timer_t* pTimer = iter->first;
        if (UV_TIMER_STATE_NONE == iter->second.iState) {
            uv_timer_init(&mstUvLoop, pTimer);
            uv_handle_set_data((uv_handle_t*)pTimer, (void*)this);
            uv_timer_start(pTimer, CUvTimer::TimerOut, iter->second.iTimeout, iter->second.iRepeat);
            iter->second.iState = UV_TIMER_STATE_RUN;
        } else if (UV_TIMER_STATE_STOP == iter->second.iState) {
            uv_close((uv_handle_t*)pTimer, NULL);
            DODELETE(pTimer);
            mmapTimer.erase(iter++);
        } else {
            LOG_ERR("Error Timer State");
        }
    }
    mcMapTimerMutex.UnLock();
}

uvtimer_handle CUvTimer::AddTimer(uint64_t iTimeout, uint64_t iRepeat, uvtimer_cb pCb, void* pParam) {
    ASSERT_RET_VALUE(pCb && mbInit, NULL);
    uv_timer_t* pTimer = (uv_timer_t*)MemMalloc(sizeof(uv_timer_t));
    tagUvTimerParam stTimer;
    BZERO(stTimer);
    stTimer.iRepeat = iRepeat;
    stTimer.iState = UV_TIMER_STATE_NONE;
    stTimer.iTimeout = iTimeout;
    stTimer.pCb = pCb;
    stTimer.pCbParam = pParam;
    stTimer.pTimer = pTimer;

    mcMapTimerMutex.Lock();
    mmapTimer.insert(std::make_pair(pTimer, stTimer));
    mcMapTimerMutex.UnLock();
    uv_async_send(&mstUvAsync);

    return (void*)pTimer;
}

int CUvTimer::DelTimer(uvtimer_handle pHandle) {
    int iRet = 1;
    mcMapTimerMutex.Lock();
    std::map<uv_timer_t*, tagUvTimerParam>::iterator iter = mmapTimer.find((uv_timer_t*)pHandle);
    if (iter != mmapTimer.end()) {
        iter->second.iState = UV_TIMER_STATE_STOP;
        iRet = 0;
    }
    mcMapTimerMutex.UnLock();

    return iRet;
}