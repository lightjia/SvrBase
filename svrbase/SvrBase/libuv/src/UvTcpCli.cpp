#include "UvTcpCli.h"

CUvTcpCli::CUvTcpCli(){
    mpTcpCli = NULL;
    mpUvConn = NULL;
	miTotalSendBytes = 0;
	miNeedSendBytes = 0;
	miTotalRecvBytes = 0;
}

CUvTcpCli::~CUvTcpCli(){
    CleanSendQueue();
}

void CUvTcpCli::RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pTcpCli){
        if (nRead == 0){
            if (uv_is_closing((uv_handle_t*)pTcpCli->mpTcpCli)){
                LOG_ERR("Cli is Closed!");
            }

            return;
        }
        
        if (nRead < 0) {
            pTcpCli->Close();
            return;
        } else if (nRead > 0) {
			pTcpCli->miTotalRecvBytes += (unsigned long)nRead;
            pTcpCli->OnRecv(nRead, pBuf);
        }
    }
}

void CUvTcpCli::ParseIpPort() {
    ASSERT_RET(NULL != mpTcpCli);
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
    Recv();
    mcSendAsyncMutex.Lock();
    uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
    uv_async_init(mpUvLoop, &mstUvSendAsync, CUvTcpCli::NotifySend);
    mcSendAsyncMutex.UnLock();

    return OnConn(0);
}

void CUvTcpCli::ConnCb(uv_connect_t* pReq, int iStatus){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pReq);
    if (NULL != pTcpCli){
        if (!iStatus) {
			pTcpCli->AfterConn();
			pTcpCli->DoSend();
		} else {
			pTcpCli->OnConn(iStatus);
		}
    }
}

int CUvTcpCli::Connect(std::string strIp, unsigned short sPort){
    ASSERT_RET_VALUE(mpTcpCli && mpUvLoop && mpUvConn, 1);

    uv_handle_set_data((uv_handle_t*)mpTcpCli, (void*)this);
    uv_tcp_init(mpUvLoop, mpTcpCli);
    if (musPort > 0) {
        int iRet = uv_tcp_bind(mpTcpCli, (struct sockaddr*)&mstLocalAddr, SO_REUSEADDR);
        if (iRet){
            LOG_ERR("uv_tcp_bind error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
            return 1;
        }
    }

    struct sockaddr_in stRemoteAddr;
    uv_ip4_addr(strIp.c_str(), sPort, &stRemoteAddr);
    uv_handle_set_data((uv_handle_t*)mpUvConn, (void*)this);
    return uv_tcp_connect(mpUvConn, mpTcpCli, (struct sockaddr*)&stRemoteAddr, CUvTcpCli::ConnCb);
}

int CUvTcpCli::Recv() {
    if (NULL == mpTcpCli || NULL == mpUvLoop) {
        return 1;
    }

    int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
    uv_recv_buffer_size((uv_handle_t*)mpTcpCli, &iSockBufLen);
    return uv_read_start((uv_stream_t*)mpTcpCli, CUvBase::UvBufAlloc, CUvTcpCli::RecvCb);
}

void CUvTcpCli::SendCb(uv_write_t* pReq, int iStatus) {
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pReq);
    if (NULL != pTcpCli){
        std::map<uv_write_t*, tagUvBufArray>::iterator iter = pTcpCli->mmapSend.find(pReq);
        if (iter != pTcpCli->mmapSend.end()) {
            uv_write_t* pWriteReq = iter->first;
			pTcpCli->MemFree(pWriteReq);
            for (unsigned int i = 0; i < iter->second.iBufNum; ++i) {
				pTcpCli->miTotalSendBytes += iter->second.pBufs[i].len;
				pTcpCli->MemFree(iter->second.pBufs[i].base);
            }

			pTcpCli->MemFree(iter->second.pBufs);
            pTcpCli->mmapSend.erase(iter);
        } else {
            LOG_ERR("Can Not Find The WriteReq");
        }

        pTcpCli->OnSend(iStatus);
        if (iStatus) {
            if (iStatus == UV_ECANCELED) {
				LOG_ERR("Peer Cancel Send Msg");
                return;
            }

            LOG_ERR("uv_write:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
            pTcpCli->Close();
        } else {
            pTcpCli->DoSend();
        }
	} else {
		LOG_ERR("uv_handle_get_data error");
	}
}

void CUvTcpCli::NotifySend(uv_async_t* pHandle){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pTcpCli){
        pTcpCli->DoSend();
    }
}

int CUvTcpCli::DoSend() {
	if (uv_is_closing((uv_handle_t*)&mpTcpCli)) {
		LOG_INFO("CUvTcpCli::DoSend() cli :%s is closing", GetNetId().c_str());
		return 0;
	}

    tagUvBufArray stBufArray;
    BZERO(stBufArray);
    mcSendMutex.Lock();
    stBufArray.iBufNum = (unsigned int)mqueSendBuf.size();
    if (stBufArray.iBufNum > 0) {
        stBufArray.pBufs = (uv_buf_t*)MemMalloc(sizeof(uv_buf_t) * stBufArray.iBufNum);
        for (unsigned int i = 0; i < stBufArray.iBufNum; ++i) {
            uv_buf_t stTmp = mqueSendBuf.front();
            mqueSendBuf.pop();
            stBufArray.pBufs[i].base = stTmp.base;
            stBufArray.pBufs[i].len = stTmp.len;
        }
    }
    mcSendMutex.UnLock();

    if (!stBufArray.iBufNum) {
        return 1;
    }

    uv_write_t* pWriteReq = (uv_write_t*)MemMalloc(sizeof(uv_write_t));
    uv_handle_set_data((uv_handle_t*)pWriteReq, (void*)this);
    mmapSend.insert(std::make_pair(pWriteReq, stBufArray));
	int iWriteRet = uv_write(pWriteReq, (uv_stream_t*)mpTcpCli, stBufArray.pBufs, stBufArray.iBufNum, CUvTcpCli::SendCb);
	if (iWriteRet) {
		LOG_ERR("uv_write:%s %s", uv_strerror(iWriteRet), uv_err_name(iWriteRet));
	}

    return iWriteRet;
}

int CUvTcpCli::Send(char* pData, ssize_t iLen){
    ASSERT_RET_VALUE(pData && iLen > 0 && mpTcpCli && mpUvLoop && uv_is_active((uv_handle_t*)&mstUvSendAsync), 1);

    uv_buf_t stTmp;
    stTmp.base = (char*)MemMalloc(iLen * sizeof(char));
    stTmp.len = (unsigned long)iLen;
	miNeedSendBytes += stTmp.len;
    memcpy(stTmp.base, pData, iLen);
    mcSendMutex.Lock();
    mqueSendBuf.push(stTmp);
    mcSendMutex.UnLock();

    mcSendAsyncMutex.Lock();
    if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
        uv_async_send(&mstUvSendAsync);
    }
    mcSendAsyncMutex.UnLock();

    return 0;
}

void CUvTcpCli::CleanSendQueue(){
    mcSendMutex.Lock();
    while (!mqueSendBuf.empty()){
        uv_buf_t stTmp = mqueSendBuf.front();
        MemFree(stTmp.base);
        mqueSendBuf.pop();
    }
    mcSendMutex.UnLock();

    while (!mmapSend.empty()) {
        std::map<uv_write_t*, tagUvBufArray>::iterator iter = mmapSend.begin();
        uv_write_t* pWriteReq = iter->first;
        MemFree(pWriteReq);
        for (unsigned int i = 0; i < iter->second.iBufNum; ++i) {
            MemFree(iter->second.pBufs[i].base);
        }

        MemFree(iter->second.pBufs);
        mmapSend.erase(iter);
    }
}

void CUvTcpCli::CloseCb(uv_handle_t* pHandle){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pTcpCli){
        pTcpCli->OnClose();
    }
}

int CUvTcpCli::Close() {
    ASSERT_RET_VALUE(mpTcpCli && mpUvLoop &&  !uv_is_closing((uv_handle_t*)mpTcpCli), 1);

    mcSendAsyncMutex.Lock();
    if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
        uv_close((uv_handle_t*)&mstUvSendAsync, NULL);
    }
    mcSendAsyncMutex.UnLock();

    uv_close((uv_handle_t*)mpTcpCli, CUvTcpCli::CloseCb);
    return 0;
}