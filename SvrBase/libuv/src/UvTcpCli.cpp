#include "UvTcpCli.h"

CUvTcpCli::CUvTcpCli(){
    mpTcpCli = nullptr;
    mpUvConn = nullptr;
    memset(&mstUvWriteReq, 0, sizeof(mstUvWriteReq));
    memset(&mstWriteBuf, 0, sizeof(mstWriteBuf));
}

CUvTcpCli::~CUvTcpCli(){
    CleanSendQueue();
}

void CUvTcpCli::RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (nullptr != pTcpCli){
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
            pTcpCli->OnRecv(nRead, pBuf);
        }
    }
}

void CUvTcpCli::ParseIpPort() {
    ASSERT_RET(nullptr != mpTcpCli);
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
    ASSERT_RET(nullptr != mpUvLoop);
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

    return 0;
}

void CUvTcpCli::ConnCb(uv_connect_t* pReq, int iStatus){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pReq);
    if (nullptr != pTcpCli){
        if (iStatus >= 0) {
            pTcpCli->AfterConn();
        }

        pTcpCli->OnConn(iStatus);
        pTcpCli->DoSend();
    }
}

int CUvTcpCli::Connect(std::string strIp, unsigned short sPort){
    if (nullptr == mpTcpCli || nullptr == mpUvLoop || nullptr == mpUvConn){
        return 1;
    }

    uv_handle_set_data((uv_handle_t*)mpTcpCli, (void*)this);
    uv_tcp_init(mpUvLoop, mpTcpCli);
    if (musPort > 0) {
        int iRet = uv_tcp_bind(mpTcpCli, (struct sockaddr*)&mstLocalAddr, SO_REUSEADDR);
        if (iRet < 0){
            LOG_ERR("uv_tcp_bind error:%s", uv_strerror(-iRet));
            return 1;
        }
    }

    struct sockaddr_in stRemoteAddr;
    uv_ip4_addr(strIp.c_str(), sPort, &stRemoteAddr);
    uv_handle_set_data((uv_handle_t*)mpUvConn, (void*)this);
    return uv_tcp_connect(mpUvConn, mpTcpCli, (struct sockaddr*)&stRemoteAddr, CUvTcpCli::ConnCb);
}

int CUvTcpCli::Recv() {
    if (nullptr == mpTcpCli || nullptr == mpUvLoop) {
        return 1;
    }

    int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
    uv_recv_buffer_size((uv_handle_t*)mpTcpCli, &iSockBufLen);
    return uv_read_start((uv_stream_t*)mpTcpCli, CUvBase::UvBufAlloc, CUvTcpCli::RecvCb);
}

void CUvTcpCli::SendCb(uv_write_t* pReq, int iStatus) {
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pReq);
    if (nullptr != pTcpCli)
    {
        pTcpCli->mcSendMutex.Lock();
        if (!pTcpCli->mqueSendBuf.empty()) {
            uv_buf_t stTmp = pTcpCli->mqueSendBuf.front();
            DOFREE(stTmp.base);
            pTcpCli->mqueSendBuf.pop();
        }
        pTcpCli->mcSendMutex.UnLock();

        pTcpCli->OnSend(iStatus);
        if (iStatus < 0) {
            if (iStatus == UV_ECANCELED) {
                return;
            }

            LOG_ERR("uv_write:%s", uv_strerror(-iStatus));
            pTcpCli->Close();
        }
        else
        {
            pTcpCli->DoSend();
        }
    }
}

void CUvTcpCli::NotifySend(uv_async_t* pHandle){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (nullptr != pTcpCli){
        pTcpCli->DoSend();
    }
}

int CUvTcpCli::DoSend() {
	if (uv_is_closing((uv_handle_t*)&mpTcpCli) || uv_is_active((uv_handle_t*)&mstUvWriteReq)) {
		return 0;
	}

    memset(&mstWriteBuf, 0, sizeof(mstWriteBuf));
    mcSendMutex.Lock();
    if (!mqueSendBuf.empty()) {
        uv_buf_t stTmp = mqueSendBuf.front();
        mstWriteBuf.base = stTmp.base;
        mstWriteBuf.len = stTmp.len;
    }
    mcSendMutex.UnLock();
    
    if (mstWriteBuf.len <= 0) {
        return 0;
    }

    uv_handle_set_data((uv_handle_t*)&mstUvWriteReq, (void*)this);
    return uv_write(&mstUvWriteReq, (uv_stream_t*)mpTcpCli, &mstWriteBuf, 1, CUvTcpCli::SendCb);
}

int CUvTcpCli::Send(char* pData, ssize_t iLen){
    if (nullptr == pData || iLen <= 0 || nullptr == mpTcpCli || nullptr == mpUvLoop || !uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
        return 1;
    }

    uv_buf_t stTmp;
    stTmp.base = (char*)do_malloc(iLen);
    stTmp.len = (unsigned long)iLen;
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
        DOFREE(stTmp.base);
        mqueSendBuf.pop();
    }
    mcSendMutex.UnLock();
}

void CUvTcpCli::CloseCb(uv_handle_t* pHandle){
    CUvTcpCli* pTcpCli = (CUvTcpCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (nullptr != pTcpCli){
        pTcpCli->OnClose();
    }
}

int CUvTcpCli::Close() {
    if (nullptr == mpTcpCli || nullptr == mpUvLoop || uv_is_closing((uv_handle_t*)mpTcpCli) != 0) {
        return 1;
    }

    mcSendAsyncMutex.Lock();
    if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
        uv_close((uv_handle_t*)&mstUvSendAsync, nullptr);
    }
    mcSendAsyncMutex.UnLock();

    uv_close((uv_handle_t*)mpTcpCli, CUvTcpCli::CloseCb);
    CleanSendQueue();
    return 0;
}