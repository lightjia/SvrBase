#ifndef __CUVLOOP__H_
#define __CUVLOOP__H_
#include "UvThread.h"
#include "UvBase.h"
#include "UvMutex.h"
#include <queue>
class CUvLoop : public CUvThread, public CUvBase
{
public:
    CUvLoop();
    ~CUvLoop();

public:
    int PushUvBase(CUvBase* pUvBase);

public:
    static void AsyncCb(uv_async_t* pHandle);

protected:
    int OnInit();
    int OnThreadRun();

private:
    void PopUvBase();

private:
    uv_loop_t mstUvLoop;
    uv_async_t mstUvAsync;
    int miBakFdNum;
    std::queue<CUvBase*> mqueUvBase;
    CUvMutex mcQueBaseMutex;
};

#endif