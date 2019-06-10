#include "UvThread.h"
#include "Log.h"

CUvThread::CUvThread(){
    mbUvThreadInit = false;
    mbQuit = false;
}

CUvThread::~CUvThread(){
    Quit();
}

void CUvThread::ThreadEntry(void *pParam) {
    if (NULL != pParam) {
        CUvThread* pUvThread = reinterpret_cast<CUvThread*>(pParam);
        if (NULL != pUvThread) {
            pUvThread->mbUvThreadInit = true;
            pUvThread->OnThreadRun();
            pUvThread->OnThreadQuit();
        }
    }
}

int CUvThread::Start() {
    ASSERT_RET_VALUE(mbUvThreadInit == false, 1);
    OnThreadCreate();
    if (uv_thread_create(&mstThread, CUvThread::ThreadEntry, (void*)this) != 0){
        LOG_ERR("uv_thread_create error");
        return 1;
    }

    while (!mbUvThreadInit) {
        sleep_ms(10);
    }

    return 0;
}

int CUvThread::Quit() {
    mbQuit = true;
    if (mbUvThreadInit) {
        uv_thread_join(&mstThread);
        mbUvThreadInit = false;
    }

    return 0;
}

int CUvThread::OnThreadCreate() {
    return 0;
}

int CUvThread::OnThreadQuit() {
    return 0;
}

void CUvThread::Wait(uint64_t iUsec) {
    if (iUsec) {
        mcUvThreadCond.TimedWait(iUsec);
    } else {
        mcUvThreadCond.Wait();
    }
}

void CUvThread::Activate() {
    mcUvThreadCond.Signal();
}