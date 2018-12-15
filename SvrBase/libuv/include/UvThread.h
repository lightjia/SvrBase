#ifndef __CUVTHREAD__H_
#define __CUVTHREAD__H_
#include "uv.h"
#include "UvSem.h"
#include "CLogmanager.h"
class CUvThread{
public:
    CUvThread();
    virtual ~CUvThread();

public:
    int Start();
    int Quit();
    void Wait();
    void Activate();

public:
    static void ThreadEntry(void *pParam);

protected:
    virtual int OnThreadRun() = 0;
    virtual int OnThreadCreate();
    virtual int OnThreadBeforeQuit();
    virtual int OnThreadQuit();
    virtual int OnThreadDestroy();

protected:
    bool mbQuit;

private:
    uv_thread_t mstThread;
    CUvSem mcUvSem;
    bool m_bInit;
};

#endif