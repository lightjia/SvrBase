#ifndef __CUVLOOP__H_
#define __CUVLOOP__H_
#include "UvThread.h"
#include "UvMutex.h"
#include "RcObject.h"
#include <queue>

class CUvLoopCb : public CRcObject {
public:
	virtual int UvCallBack(uv_loop_t*, void*) = 0;
};

struct tagUvLoopCb {
	CUvLoopCb* pUvLoopCb;
	void* pCbData;
};

class CUvLoop : public CUvThread{
public:
    CUvLoop();
    ~CUvLoop();

public:
    int CallUv(CUvLoopCb* pUvLoopCb, void* pCbData = NULL);
	int StartLoop();

public:
    static void AsyncCb(uv_async_t* pHandle);

protected:
    int OnThreadRun();

private:
    void UvCb();

private:
	uv_loop_t *mpUvLoop;
    uv_loop_t mstUvLoop;
    uv_async_t mstUvAsync;
    int miBakFdNum;
    std::queue<tagUvLoopCb> mqueUvLoopCbs;
    CUvMutex mcUvLoopCbsMutex;
};

#endif