#include "UvTaskThread.h"
#include "UvTaskPool.h"

CUvTaskThread::CUvTaskThread(){
    mpTask = NULL;
}

CUvTaskThread::~CUvTaskThread(){
}

int CUvTaskThread::OnThreadRun() {
    for (;;) {
        if (NULL == mpTask) {
            sUvTaskPool->PushTaskThread(this);
            Wait();
            continue;
        }
        
        mpTask->TaskInit();
        mpTask->TaskExcute();
        mpTask->TaskQuit();
        UNREF(mpTask);
        mpTask = NULL;
    }

    return 0;
}

int CUvTaskThread::SetTask(CTask* pTask) {
    ASSERT_RET_VALUE(NULL != pTask && NULL == mpTask, 1);
    mpTask = pTask;
    REF(mpTask);
    return 0;
}