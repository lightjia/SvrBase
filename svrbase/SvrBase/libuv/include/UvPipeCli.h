#ifndef __CUVPIPECLI__H_
#define __CUVPIPECLI__H_
#include "UvPipeBase.h"
#include "UvMutex.h"
class CUvPipeCli : public CUvPipeBase{
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
	int Close();
	uint64_t GetTotalSendBytes() { return miTotalSendBytes; }
	uint64_t GetTotalRecvBytes() { return miTotalRecvBytes; }
	uint64_t GetNeedSendBytes() { return miNeedSendBytes; }

private:
	int AfterConn();
	int StartRecv();
	int DoSend();
	void DoConn(int iStatus);
	void DoRecv(ssize_t nRead, const uv_buf_t* pBuf);
	void AfterSend(uv_write_t* pReq, int iStatus);
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
	int miTcpCliState;
	uint64_t miTotalRecvBytes;
	uint64_t miTotalSendBytes;
	uint64_t miNeedSendBytes;
	CMemBuffer* mpSendBuf;
	uv_async_t mstUvSendAsync;
	CUvMutex mcSendMutex;
	std::map<uv_write_t*, tagUvBufArray> mmapSend;
};

#endif