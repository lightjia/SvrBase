#ifndef __CUVPIPECLI__H_
#define __CUVPIPECLI__H_
#include "UvPipeBase.h"
#include "UvMutex.h"
class CUvPipeCli : public CUvPipeBase
{
public:
    CUvPipeCli();
    virtual ~CUvPipeCli();

public:
    static void RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf);
    static void ConnCb(uv_connect_t* pReq, int iStatus);
    static void SendCb(uv_write_t* pReq, int iStatus);
    static void CloseCb(uv_handle_t* pHandle);
    static void NotifySend(uv_async_t* pHandle);

public:
    void SetPipeCli(uv_pipe_t* pPipeCli);
    int Connect();
    int Send(char* pData, ssize_t iLen);

protected:
    int Close();

private:
    int AfterConn();
    int Recv();
    int DoSend();
    void CleanSendQueue();

protected:
    virtual int OnRecv(ssize_t nRead, const uv_buf_t* pBuf) = 0;
    virtual int OnConn(int iStatus) = 0;
    virtual int OnClose() = 0;
    virtual int OnSend(int iStatus) = 0;

protected:
    uv_pipe_t* mpPipeCli;
    uv_connect_t* mpUvConn;

private:
    uv_async_t mstUvSendAsync;
    std::queue<uv_buf_t> mqueSendBuf;
    CUvMutex mcSendMutex;
    uv_write_t mstUvWriteReq;
    uv_buf_t mstWriteBuf;
};

#endif