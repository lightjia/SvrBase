#include "UvTaskPool.h"

CUvTaskPool::CUvTaskPool(){
}

CUvTaskPool::~CUvTaskPool(){
}

int CUvTaskPool::OnThreadRun() {
    for (;;) {
        CTask* pTask = PopTask();
        if (nullptr == pTask) {
            Wait();
            continue;
        }

        DispatchTask(pTask);
    }
    return 0;
}

CTask* CUvTaskPool::PopTask() {
    CTask* pTask = nullptr;
    mcQueTasksMutex.Lock();
    if (!mqueTasks.empty()) {
        pTask = mqueTasks.front();
        mqueTasks.pop();
    }
    mcQueTasksMutex.UnLock();

    return pTask;
}

int CUvTaskPool::PushTask(CTask* pTask) {
    ASSERT_RET_VALUE(nullptr != pTask, 1);
    mcQueTasksMutex.Lock();
    mqueTasks.push(pTask);
    mcQueTasksMutex.UnLock();

    Activate();
    return 0;
}

int CUvTaskPool::PushTaskThread(CUvTaskThread* pTaskThread) {
    ASSERT_RET_VALUE(nullptr != pTaskThread, 1);
    mcTaskThreadsMutex.Lock();
    msetTaskThreads.insert(pTaskThread);
    mcTaskThreadsMutex.UnLock();
    return 0;
}

int CUvTaskPool::DispatchTask(CTask* pTask) {
    ASSERT_RET_VALUE(nullptr != pTask, 1);
    CUvTaskThread* pTaskThread = nullptr;
    mcTaskThreadsMutex.Lock();
    std::set<CUvTaskThread*>::iterator iter = msetTaskThreads.begin();
    if (iter != msetTaskThreads.end()) {
        pTaskThread = (CUvTaskThread*)*iter;
        if (nullptr != pTaskThread) {
            pTaskThread->SetTask(pTask);
            pTaskThread->Activate();
        }

        msetTaskThreads.erase(iter);
    }
    mcTaskThreadsMutex.UnLock();

    if (nullptr == pTaskThread) {
        pTaskThread = new CUvTaskThread();
        ASSERT_RET_VALUE(nullptr != pTaskThread, 1);
        pTaskThread->SetTask(pTask);
        pTaskThread->Start();
    }

    return 0;
}