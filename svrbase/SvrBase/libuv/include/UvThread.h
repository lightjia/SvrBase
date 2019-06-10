#ifndef __CUVTHREAD__H_
#define __CUVTHREAD__H_
#include "uv.h"
#include "UvCond.h"

class CUvThread{
public:
    CUvThread();
    virtual ~CUvThread();

public:
    int Start();
    int Quit();
    void Wait(uint64_t iUsec = 0);
    void Activate();

public:
    static void ThreadEntry(void *pParam);

protected:
    virtual int OnThreadRun() = 0;
    virtual int OnThreadCreate();
    virtual int OnThreadQuit();

protected:
    bool mbQuit;

private:
    uv_thread_t mstThread;
    CUvCond mcUvThreadCond;
    bool mbUvThreadInit;
};

#endif