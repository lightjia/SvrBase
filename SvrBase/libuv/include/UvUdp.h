#ifndef __CUVUDP__H_
#define __CUVUDP__H_
#include "UvNetBase.h"

struct tagUvUdpPkg {
    struct sockaddr_in stAddr;
    uv_buf_t stBuf;
};

class CUvUdp : public CUvNetBase{
public:
    CUvUdp();
    virtual ~CUvUdp();

public:
    static void RecvCb(uv_udp_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf, const struct sockaddr* pAddr, unsigned iFlag);
    static void NotifySend(uv_async_t* pHandle);
    static void SendCb(uv_udp_send_t* pReq, int iStatus);

public:
    int Start();
    int Close();
    int Send(char* pData, ssize_t iLen, std::string strIp, unsigned short uPort);
    int Send(char* pData, ssize_t iLen, const struct sockaddr* pAddr);

protected:
    virtual int OnRecv(ssize_t nRead, const uv_buf_t* pBuf, const struct sockaddr* pAddr, unsigned iFlag) = 0;
    virtual int OnSend(int iStatus) = 0;

private:
    int DoSend();
    void CleanSendQueue();

protected:
    uv_udp_t* mpUdp;

private:
    uv_async_t mstUvSendAsync;
    std::queue<tagUvUdpPkg> mqueSendBuf;
    CUvMutex mcSendMutex;
    uv_udp_send_t mstUvWriteReq;
    uv_buf_t mstWriteBuf;
};

#endif