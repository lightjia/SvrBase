#include "UvThread.h"
#include "Log.h"

CUvThread::CUvThread(){
    m_bInit = false;
    mbQuit = false;
}

CUvThread::~CUvThread(){
    Quit();
}

void CUvThread::ThreadEntry(void *pParam) {
    if (NULL != pParam) {
        CUvThread* pUvThread = reinterpret_cast<CUvThread*>(pParam);
        if (NULL != pUvThread) {
            pUvThread->m_bInit = true;
            pUvThread->OnThreadRun();
            pUvThread->OnThreadQuit();
        }
    }
}

int CUvThread::Start() {
    ASSERT_RET_VALUE(m_bInit == false, 1);
    OnThreadCreate();
    if (uv_thread_create(&mstThread, CUvThread::ThreadEntry, (void*)this) != 0){
        LOG_ERR("uv_thread_create error");
        return 1;
    }

    while (!m_bInit) {
        sleep_ms(10);
    }

    return 0;
}

int CUvThread::Quit() {
    mbQuit = true;
    if (m_bInit) {
        OnThreadBeforeQuit();
        uv_thread_join(&mstThread);
        m_bInit = false;
        return OnThreadDestroy();
    }

    return 0;
}

int CUvThread::OnThreadCreate() {
    LOG_INFO("Enter CUvThread::OnThreadCreate");
    return 0;
}

int CUvThread::OnThreadBeforeQuit() {
    LOG_INFO("Enter CUvThread::OnThreadBeforeQuit");
    return 0;
}

int CUvThread::OnThreadQuit() {
    LOG_INFO("Enter CUvThread::OnThreadQuit");
    return 0;
}

int CUvThread::OnThreadDestroy() {
    LOG_INFO("Enter CUvThread::OnThreadDestroy");
    return 0;
}

void CUvThread::Wait(uint64_t iUsec) {
    if (iUsec) {
        mcUvCond.TimedWait(iUsec);
    } else {
        mcUvCond.Wait();
    }
}

void CUvThread::Activate() {
    mcUvCond.Signal();
}