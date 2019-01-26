#ifndef __CUVTASKTHREAD__H_
#define __CUVTASKTHREAD__H_
#include "UvThread.h"
#include "Task.h"
class CUvTaskThread : public CUvThread
{
public:
    CUvTaskThread();
    ~CUvTaskThread();

public:
    int SetTask(CTask* pTask);

protected:
    int OnThreadRun();
    CTask* mpTask;
};

#endif