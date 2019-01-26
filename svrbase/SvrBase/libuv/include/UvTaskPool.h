#ifndef __CUVTASKPOOL__H_
#define __CUVTASKPOOL__H_
#include "singleton.h"
#include "UvTaskThread.h"
#include "UvMutex.h"
#include <set>
#include <queue>
class CUvTaskPool : public CSingleton<CUvTaskPool>, public CUvThread
{
    SINGLE_CLASS_INITIAL(CUvTaskPool);
public:
    ~CUvTaskPool();

public:
    int Init();
    int UnInit();
    int PushTask(CTask* pTask);
    int PushTaskThread(CUvTaskThread* pTaskThread);

private:
    CTask* PopTask();
    int DispatchTask(CTask* pTask);

protected:
    int OnThreadRun();

private:
    bool mbStart;
    std::queue<CTask*> mqueTasks;
    CUvMutex mcQueTasksMutex;

    std::set<CUvTaskThread*> msetTaskThreads;
    CUvMutex mcTaskThreadsMutex;
};

#define sUvTaskPool CUvTaskPool::Instance()
#endif