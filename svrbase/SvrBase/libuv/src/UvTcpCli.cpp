#include "UvTcpCli.h"

enum UV_TCP_CLI_STATE {
	UV_TCP_CLI_STATE_NONE,
	UV_TCP_CLI_STATE_ESTAB,
	UV_TCP_CLI_STATE_CLOSE,
};

CUvTcpCli::CUvTcpCli(){
    mpTcpCli = NULL;
    mpUvConn = NULL;
	mpSendBuf = NULL;
	miTotalSendBytes = 0;
	miNeedSendBytes = 0;
	miTotalRecvBytes = 0;
	miTcpCliState = UV_TCP_CLI_STATE_NONE;
}

CUvTcpCli::~CUvTcpCli(){
    CleanSendQueue();
}

void CUvTcpCli::DoRecv(ssize_t nRead, const uv_buf_t* pBuf) {
	if (nRead == 0) {
		LOG_DBG("%s recv 0 bytes", GetNetId().c_str());
	} else if (nRead < 0) {
		Close();
	} else if (nRead > 0) {
		miTotalRecvBytes += (unsigned long)nRead;
		OnRecv(nRead, pBuf);
	}
}

void CUvTcpCli::RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
	ASSERT_RET(pTcpCli);
	pTcpCli->DoRecv(nRead, pBuf);
}

void CUvTcpCli::ParseIpPort() {
    ASSERT_RET(mpTcpCli);
    struct sockaddr stSock;
    int iSockNameLen = (int)sizeof(stSock);
    uv_tcp_getpeername(mpTcpCli, &stSock, &iSockNameLen);

    struct sockaddr_in *pSock = (struct sockaddr_in*)&stSock;
    musPort = ntohs(pSock->sin_port);
#if (defined PLATFORM_WINDOWS)
    mstrIp = inet_ntoa(pSock->sin_addr);
#elif  (defined PLATFORM_LINUX)
    struct in_addr in = pSock->sin_addr;
    char str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
    inet_ntop(AF_INET, &in, str, sizeof(str));
    mstrIp = str;
#endif
}

void CUvTcpCli::SetTcpCli(uv_tcp_t* pTcpCli) {
    ASSERT_RET(NULL != mpUvLoop);
    mpTcpCli = pTcpCli;
    ParseIpPort();
    uv_handle_set_data((uv_handle_t*)mpTcpCli, (void*)this);
    AfterConn();
}

int CUvTcpCli::AfterConn() {
	StartRecv();

	mcSendMutex.Lock();
    uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
    uv_async_init(mpUvLoop, &mstUvSendAsync, CUvTcpCli::NotifySend);
	miTcpCliState = UV_TCP_CLI_STATE_ESTAB;
	mcSendMutex.UnLock();

    return OnConn(0);
}

void CUvTcpCli::DoConn(int iStatus) {
	if (!iStatus) {
		AfterConn();
		DoSend();
	} else {
		LOG_ERR("uv_tcp_connect error:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
		OnConn(iStatus);
		uv_close((uv_handle_t*)mpTcpCli, NULL);
	}
}

void CUvTcpCli::ConnCb(uv_connect_t* pReq, int iStatus){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pReq);
	ASSERT_RET(pTcpCli);
	pTcpCli->DoConn(iStatus);
}

int CUvTcpCli::Connect(std::string strIp, unsigned short sPort){
	int iRet = 1;
    ASSERT_RET_VALUE(mpTcpCli && mpUvLoop && mpUvConn, iRet);

    uv_handle_set_data((uv_handle_t*)mpTcpCli, (void*)this);
    uv_tcp_init(mpUvLoop, mpTcpCli);
    if (musPort > 0) {
        iRet = uv_tcp_bind(mpTcpCli, (struct sockaddr*)&mstLocalAddr, SO_REUSEADDR);
        if (iRet){
            LOG_ERR("uv_tcp_bind error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
            return iRet;
        }
    }

    struct sockaddr_in stRemoteAddr;
    uv_ip4_addr(strIp.c_str(), sPort, &stRemoteAddr);
    uv_handle_set_data((uv_handle_t*)mpUvConn, (void*)this);
	iRet = uv_tcp_connect(mpUvConn, mpTcpCli, (struct sockaddr*)&stRemoteAddr, CUvTcpCli::ConnCb);
	if (iRet) {
		LOG_ERR("uv_tcp_connect error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
	}

    return iRet;
}

int CUvTcpCli::StartRecv() {
	ASSERT_RET_VALUE(mpTcpCli && mpUvLoop, 1);

    int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
    uv_recv_buffer_size((uv_handle_t*)mpTcpCli, &iSockBufLen);
    return uv_read_start((uv_stream_t*)mpTcpCli, CUvBase::UvBufAlloc, CUvTcpCli::RecvCb);
}

void CUvTcpCli::AfterSend(uv_write_t* pReq, int iStatus) {
	std::map<uv_write_t*, tagUvBufArray>::iterator iter = mmapSend.find(pReq);
	if (iter != mmapSend.end()) {
		uv_write_t* pWriteReq = iter->first;
		MemFree(pWriteReq);
		miTotalSendBytes += iter->second.pMemBuf->GetBuffLen();
		DODELETE(iter->second.pMemBuf);
		MemFree(iter->second.pBufs);
		mmapSend.erase(iter);
	} else {
		LOG_ERR("Can Not Find The WriteReq");
	}

	OnSend(iStatus);
	if (iStatus) {
		if (iStatus == UV_ECANCELED) {
			LOG_ERR("Peer Cancel Send Msg");
			return;
		}

		LOG_ERR("uv_write:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
		Close();
	} else {
		DoSend();
	}
}

void CUvTcpCli::SendCb(uv_write_t* pReq, int iStatus) {
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pReq);
	ASSERT_RET(pTcpCli);
	pTcpCli->AfterSend(pReq, iStatus);
}

void CUvTcpCli::NotifySend(uv_async_t* pHandle){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pTcpCli){
        pTcpCli->DoSend();
    }
}

int CUvTcpCli::DoSend() {
	CAutoMutex cAutoMutex(&mcSendMutex);
	if (UV_TCP_CLI_STATE_ESTAB == miTcpCliState) {
		tagUvBufArray stBufArray;
		BZERO(stBufArray);
		if (mpSendBuf && mpSendBuf->GetBuffLen() > 0) {
			stBufArray.iBufNum = 1;
			stBufArray.pBufs = (uv_buf_t*)MemMalloc(sizeof(uv_buf_t));
			stBufArray.pBufs->base = (char*)mpSendBuf->GetBuffer();
			stBufArray.pBufs->len = (unsigned long)mpSendBuf->GetBuffLen();
			stBufArray.pMemBuf = mpSendBuf;
			mpSendBuf = NULL;
		}

		if (stBufArray.iBufNum > 0) {
			uv_write_t* pWriteReq = (uv_write_t*)MemMalloc(sizeof(uv_write_t));
			uv_handle_set_data((uv_handle_t*)pWriteReq, (void*)this);
			mmapSend.insert(std::make_pair(pWriteReq, stBufArray));
			int iWriteRet = uv_write(pWriteReq, (uv_stream_t*)mpTcpCli, stBufArray.pBufs, stBufArray.iBufNum, CUvTcpCli::SendCb);
			if (iWriteRet) {
				LOG_ERR("uv_write:%s %s", uv_strerror(iWriteRet), uv_err_name(iWriteRet));
			}
		}
	}

    return 0;
}

int CUvTcpCli::Send(char* pData, ssize_t iLen){
    ASSERT_RET_VALUE(pData && iLen > 0 && mpTcpCli && mpUvLoop && UV_TCP_CLI_STATE_CLOSE != miTcpCliState, 1);
	CAutoMutex cAutoMutex(&mcSendMutex);
	if (!mpSendBuf) {
		mpSendBuf = new CMemBuffer();
	}

	ASSERT_RET_VALUE(mpSendBuf, 1);
	miNeedSendBytes += iLen;
	mpSendBuf->Append(pData, iLen);
	if (UV_TCP_CLI_STATE_ESTAB == miTcpCliState) {
		uv_async_send(&mstUvSendAsync);
	}

    return 0;
}

void CUvTcpCli::CleanSendQueue(){
	CAutoMutex cAutoMutex(&mcSendMutex);
	DODELETE(mpSendBuf);

    while (!mmapSend.empty()) {
        std::map<uv_write_t*, tagUvBufArray>::iterator iter = mmapSend.begin();
        uv_write_t* pWriteReq = iter->first;
        MemFree(pWriteReq);
		DODELETE(iter->second.pMemBuf);
        MemFree(iter->second.pBufs);
        mmapSend.erase(iter);
    }
}

void CUvTcpCli::CloseCb(uv_handle_t* pHandle){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (pTcpCli){
        pTcpCli->OnClose();
		//use uv_is_active to panduan
		uv_close((uv_handle_t*)&pTcpCli->mstUvSendAsync, NULL);
    }
}

int CUvTcpCli::Close() {
    ASSERT_RET_VALUE(mpTcpCli && mpUvLoop, 1);
	CAutoMutex cAutoMutex(&mcSendMutex);
	if (UV_TCP_CLI_STATE_ESTAB == miTcpCliState) {
		uv_close((uv_handle_t*)mpTcpCli, CUvTcpCli::CloseCb);
		miTcpCliState = UV_TCP_CLI_STATE_CLOSE;
	}

    return 0;
}