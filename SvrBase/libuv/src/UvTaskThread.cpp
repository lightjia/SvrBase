#include "UvTaskThread.h"
#include "UvTaskPool.h"

CUvTaskThread::CUvTaskThread(){
    mpTask = nullptr;
}

CUvTaskThread::~CUvTaskThread(){
}

int CUvTaskThread::OnThreadRun() {
    for (;;) {
        if (nullptr == mpTask) {
            sUvTaskPool->PushTaskThread(this);
            Wait();
            continue;
        }
        
        mpTask->TaskInit();
        mpTask->TaskExcute();
        mpTask->TaskQuit();
        UNREF(mpTask);
        mpTask = nullptr;
    }

    return 0;
}

int CUvTaskThread::SetTask(CTask* pTask) {
    ASSERT_RET_VALUE(nullptr != pTask && nullptr == mpTask, 1);
    mpTask = pTask;
    REF(mpTask);
    return 0;
}