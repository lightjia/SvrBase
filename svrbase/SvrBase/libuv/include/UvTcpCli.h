#ifndef __CUVTCPCLI__H_
#define __CUVTCPCLI__H_
#include "UvNetBase.h"
#include <queue>
#include <map>
#include <vector>

enum UV_TCP_CLI_STATE {
	UV_TCP_CLI_STATE_NONE,
	UV_TCP_CLI_STATE_ESTAB,
	UV_TCP_CLI_STATE_CLOSE,
};

class CUvTcpCli : public CUvNetBase{
public:
    CUvTcpCli();
    virtual ~CUvTcpCli();

public:
    static void RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf);
    static void ConnCb(uv_connect_t* pReq, int iStatus);
    static void SendCb(uv_write_t* pReq, int iStatus);
    static void CloseCb(uv_handle_t* pHandle);
    static void NotifySend(uv_async_t* pHandle);

public:
    void SetTcpCli(uv_tcp_t* pTcpCli);
    int Connect(std::string strIp, unsigned short sPort);
    int Send(char* pData, ssize_t iLen);
    int Close();
	uint64_t GetTotalSendBytes() { return miTotalSendBytes; }
	uint64_t GetTotalRecvBytes() { return miTotalRecvBytes; }
	uint64_t GetNeedSendBytes() { return miNeedSendBytes; }

private:
    void ParseIpPort();
    int AfterConn();
    int StartRecv();
    int DoSend();
    void CleanSendQueue();

protected:
    virtual int OnRecv(ssize_t nRead, const uv_buf_t* pBuf) = 0;
    virtual int OnConn(int iStatus) = 0;
    virtual int OnClose() = 0;
    virtual int OnSend(int iStatus) = 0;

protected:
    uv_tcp_t* mpTcpCli;
    uv_connect_t* mpUvConn;

private:
	UV_TCP_CLI_STATE miTcpCliState;
	uint64_t miTotalRecvBytes;
	uint64_t miTotalSendBytes;
	uint64_t miNeedSendBytes;
    uv_async_t mstUvSendAsync;
    std::queue<uv_buf_t> mqueSendBuf;
    CUvMutex mcSendMutex;
    std::map<uv_write_t*, tagUvBufArray> mmapSend;
};

#endif