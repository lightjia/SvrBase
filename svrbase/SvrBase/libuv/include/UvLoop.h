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
    CUvLoop(uv_run_mode iRunMode = UV_RUN_DEFAULT, uv_loop_t* pUvLoop = NULL);
    ~CUvLoop();

public:
    int CallUv(CUvLoopCb* pUvLoopCb, void* pCbData = NULL);
	int StartLoop();
	const int GetFdNum() { return miBakFdNum; }
	uv_loop_t* GetUvLoop() { return mpUvLoop; }

public:
    static void AsyncCb(uv_async_t* pHandle);

protected:
    int OnThreadRun();

private:
    void UvCb();

private:
	uv_loop_t *mpUvLoop;
	uv_loop_t mstUvLoop;
	uv_run_mode miUvRunMode;
    uv_async_t mstUvAsync;
    int miBakFdNum;
    std::queue<tagUvLoopCb> mqueUvLoopCbs;
    CUvMutex mcUvLoopCbsMutex;
};

#endif