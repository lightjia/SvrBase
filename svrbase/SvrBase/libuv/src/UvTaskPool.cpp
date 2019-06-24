#include "UvTaskPool.h"

CUvTaskPool::CUvTaskPool(){
    mbStart = false;
	miTaskThreadNum = 0;
	miTaskNum = 0;
}

CUvTaskPool::~CUvTaskPool(){
}

int CUvTaskPool::Init() {
    ASSERT_RET_VALUE(!mbStart, 1);
    mbStart = true;

    return Start();
}

int CUvTaskPool::UnInit() {
    mbStart = false;

    return 0;
}

int CUvTaskPool::OnThreadRun() {
    for (;;) {
        CTask* pTask = PopTask();
        if (NULL == pTask) {
            Wait();
            continue;
        }

        DispatchTask(pTask);
    }
    return 0;
}

CTask* CUvTaskPool::PopTask() {
    CTask* pTask = NULL;
    mcQueTasksMutex.Lock();
    if (!mqueTasks.empty()) {
        pTask = mqueTasks.front();
        mqueTasks.pop();
    }
    mcQueTasksMutex.UnLock();

    return pTask;
}

int CUvTaskPool::PushTask(CTask* pTask) {
    ASSERT_RET_VALUE(NULL != pTask && mbStart, 1);
    REF(pTask);
    mcQueTasksMutex.Lock();
    mqueTasks.push(pTask);
	miTaskNum++;
    mcQueTasksMutex.UnLock();

    Activate();
    return 0;
}

int CUvTaskPool::PushTaskThread(CUvTaskThread* pTaskThread) {
    ASSERT_RET_VALUE(NULL != pTaskThread, 1);
    mcTaskThreadsMutex.Lock();
    msetTaskThreads.insert(pTaskThread);
    mcTaskThreadsMutex.UnLock();
    return 0;
}

int CUvTaskPool::DispatchTask(CTask* pTask) {
    ASSERT_RET_VALUE(NULL != pTask, 1);
    CUvTaskThread* pTaskThread = NULL;
    mcTaskThreadsMutex.Lock();
    std::set<CUvTaskThread*>::iterator iter = msetTaskThreads.begin();
    if (iter != msetTaskThreads.end()) {
        pTaskThread = (CUvTaskThread*)*iter;
        msetTaskThreads.erase(iter);
    }
    mcTaskThreadsMutex.UnLock();

    if (NULL == pTaskThread) {
        pTaskThread = new CUvTaskThread();
		miTaskThreadNum++;
        ASSERT_RET_VALUE(NULL != pTaskThread, 1);
        pTaskThread->SetTask(pTask);
        pTaskThread->Start();
    } else {
        pTaskThread->SetTask(pTask);
        pTaskThread->Activate();
    }

    UNREF(pTask);
    return 0;
}