#include "UvUdp.h"

CUvUdp::CUvUdp(){
    mpUdp = nullptr;
}

CUvUdp::~CUvUdp(){
    if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
        uv_close((uv_handle_t*)&mstUvSendAsync, nullptr);
    }

    CleanSendQueue();
}

void CUvUdp::RecvCb(uv_udp_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf, const struct sockaddr* pAddr, unsigned iFlag) {
    if (nRead <= 0){
        return;
    }

    CUvUdp* pUdpSvr = (CUvUdp*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (nullptr != pUdpSvr) {
        pUdpSvr->OnRecv(nRead, pBuf, pAddr, iFlag);
    }
}

void CUvUdp::NotifySend(uv_async_t* pHandle) {
    CUvUdp* pUdp = (CUvUdp*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (nullptr != pUdp) {
        pUdp->DoSend();
    }
}

int CUvUdp::Send(char* pData, ssize_t iLen, std::string strIp, unsigned short uPort) {
    ASSERT_RET_VALUE(nullptr != mpUdp && nullptr != mpUvLoop, 1);

    struct sockaddr_in stAddr;
    uv_ip4_addr(strIp.c_str(), uPort, &stAddr);
    return Send(pData, iLen, (const struct sockaddr*)&stAddr);
}

int CUvUdp::Send(char* pData, ssize_t iLen, const struct sockaddr* pAddr) {
    ASSERT_RET_VALUE(nullptr != mpUdp && nullptr != mpUvLoop && nullptr != pAddr, 1);
    tagUvUdpPkg stTmp;
    stTmp.stBuf.base = (char*)do_malloc(iLen);
    stTmp.stBuf.len = (unsigned long)iLen;
    memcpy(stTmp.stBuf.base, pData, iLen);
    memcpy(&stTmp.stAddr, pAddr, sizeof(stTmp.stAddr));

    mcSendMutex.Lock();
    mqueSendBuf.push(stTmp);
    mcSendMutex.UnLock();
    return uv_async_send(&mstUvSendAsync);
}

void CUvUdp::SendCb(uv_udp_send_t* pReq, int iStatus) {
    if (iStatus < 0) {
        LOG_ERR("Udp Send Error");
    }

    CUvUdp* pUdp = (CUvUdp*)uv_handle_get_data((uv_handle_t*)pReq);
    if (nullptr != pUdp) {
        pUdp->mcSendMutex.Lock();
        tagUvUdpPkg stTmp = pUdp->mqueSendBuf.front();
        DOFREE(stTmp.stBuf.base);
        pUdp->mqueSendBuf.pop();
        pUdp->mcSendMutex.UnLock();

        pUdp->OnSend(iStatus);
        pUdp->DoSend();
    }
}

int CUvUdp::DoSend() {
    memset(&mstWriteBuf, 0, sizeof(mstWriteBuf));
    tagUvUdpPkg stTmp;
    mcSendMutex.Lock();
    if (!mqueSendBuf.empty()) {
        stTmp = mqueSendBuf.front();
        mstWriteBuf.base = stTmp.stBuf.base;
        mstWriteBuf.len = stTmp.stBuf.len;
    }
    mcSendMutex.UnLock();

    if (mstWriteBuf.len <= 0) {
        return 0;
    }

    uv_handle_set_data((uv_handle_t*)&mstUvWriteReq, (void*)this);
    return uv_udp_send(&mstUvWriteReq, mpUdp, &mstWriteBuf, 1, (struct sockaddr*)&stTmp.stAddr, CUvUdp::SendCb);
}

int CUvUdp::Start() {
    ASSERT_RET_VALUE(nullptr != mpUvLoop && nullptr != mpUdp, 1);
    uv_udp_init(mpUvLoop, mpUdp);
    uv_handle_set_data((uv_handle_t*)mpUdp, (void*)this);
    if (musPort > 0) {
        int iRet = uv_udp_bind(mpUdp, (struct sockaddr*)&mstLocalAddr, UV_UDP_REUSEADDR);
        if (iRet < 0) {
            LOG_ERR("uv_udp_bind error:%s,port(%d)", uv_strerror(-iRet), mstLocalAddr.sin_port);
            return 1;
        }
    }
    else {
        uv_udp_set_broadcast(mpUdp, 1);
    }

    int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
    uv_recv_buffer_size((uv_handle_t*)mpUdp, &iSockBufLen);
    uv_udp_recv_start(mpUdp, CUvBase::UvBufAlloc, CUvUdp::RecvCb);

    uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
    uv_async_init(mpUvLoop, &mstUvSendAsync, CUvUdp::NotifySend);

    return DoSend();
}

void CUvUdp::CleanSendQueue() {
    mcSendMutex.Lock();
    while (!mqueSendBuf.empty()) {
        tagUvUdpPkg stTmp = mqueSendBuf.front();
        DOFREE(stTmp.stBuf.base);
        mqueSendBuf.pop();
    }
    mcSendMutex.UnLock();
}

int CUvUdp::Close() {
    ASSERT_RET_VALUE(nullptr != mpUvLoop && nullptr != mpUdp, 1);
    return 0;
}