#ifndef __CUVTCPSVR__H_
#define __CUVTCPSVR__H_
#include "UvNetBase.h"
class CUvTcpSvr : public CUvNetBase{
public:
    CUvTcpSvr();
    virtual ~CUvTcpSvr();

public:
    static void ConnCb(uv_stream_t* pHandle, int iStatus);

protected:
    virtual int OnAccept(uv_tcp_t* pUvTcp) = 0;//TODO free pUvTcp
    virtual uv_tcp_t* AllocTcpCli();
	virtual int FreeTcpCli(uv_tcp_t* pTcpCli);

protected:
    int Listen(int iBackLog = 5);

private:
	void DoConn(int iStatus);

protected:
    uv_tcp_t* mpTcpSvr;
};

#endif