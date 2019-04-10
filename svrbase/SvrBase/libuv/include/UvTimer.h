#ifndef __CUVTIMER__H_
#define __CUVTIMER__H_
#include "singleton.h"
#include "UvThread.h"
#include "UvMutex.h"
#include "MemOper.h"
#include <map>
#include <queue>

typedef void (*uvtimer_cb)(void* pParam);
typedef void* uvtimer_handle;
enum UV_TIMER_STATE {
    UV_TIMER_STATE_NONE = 1,
    UV_TIMER_STATE_RUN,
    UV_TIMER_STATE_STOP,
};

#pragma pack(1)
struct tagUvTimerParam{
    uvtimer_cb pCb;
    void* pCbParam;
    UV_TIMER_STATE iState;
    uint64_t iTimeout;
    uint64_t iRepeat;
    uv_timer_t* pTimer;
};
#pragma pack()

class CUvTimer : public CSingleton<CUvTimer>, public CUvThread, public CMemOper{
    SINGLE_CLASS_INITIAL(CUvTimer);

public:
    ~CUvTimer();

public:
    int Init();
    int UnInit();
    uvtimer_handle AddTimer(uint64_t iTimeout, uint64_t iRepeat, uvtimer_cb pCb, void* pParam = NULL);
    int DelTimer(uvtimer_handle pHandle);

public:
    static void AsyncCb(uv_async_t* pHandle);
    static void TimerOut(uv_timer_t* pHandler);

protected:
    int OnThreadRun();

private:
    void HandleTimer();

private:
    std::map<uv_timer_t*, tagUvTimerParam> mmapTimer;
    CUvMutex mcMapTimerMutex;
    uv_loop_t mstUvLoop;
    uv_async_t mstUvAsync;
    bool mbInit;
};

#define sUvTimer CUvTimer::Instance()
#endif