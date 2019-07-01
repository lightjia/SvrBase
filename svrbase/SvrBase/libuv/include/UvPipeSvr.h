#ifndef __CUVPIEPESVR__H_
#define __CUVPIEPESVR__H_
#include "UvPipeBase.h"
class CUvPipeSvr : public CUvPipeBase{
public:
    CUvPipeSvr();
    virtual ~CUvPipeSvr();

public:
    static void ConnCb(uv_stream_t* pHandle, int iStatus);

protected:
    virtual int OnAccept(uv_pipe_t* pUvPipe) = 0;//TODO free pUvPipe
    virtual uv_pipe_t* AllocPipeCli();
	virtual int FreePipeCli(uv_pipe_t* pPipeCli);

protected:
    int Listen(int iBackLog = 5);

private:
	void DoConn(int iStatus);

protected:
    uv_pipe_t* mpPipeSvr;
};

#endif