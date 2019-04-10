#include "UvUdp.h"

CUvUdp::CUvUdp(){
    mpUdp = NULL;
}

CUvUdp::~CUvUdp(){
    if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
        uv_close((uv_handle_t*)&mstUvSendAsync, NULL);
    }

    CleanSendQueue();
}

void CUvUdp::RecvCb(uv_udp_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf, const struct sockaddr* pAddr, unsigned iFlag) {
    if (nRead <= 0){
        return;
    }

    CUvUdp* pUdpSvr = (CUvUdp*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pUdpSvr) {
        pUdpSvr->OnRecv(nRead, pBuf, pAddr, iFlag);
    }
}

void CUvUdp::NotifySend(uv_async_t* pHandle) {
    CUvUdp* pUdp = (CUvUdp*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pUdp) {
        pUdp->DoSend();
    }
}

int CUvUdp::Send(char* pData, ssize_t iLen, std::string strIp, unsigned short uPort) {
    ASSERT_RET_VALUE(NULL != mpUdp && NULL != mpUvLoop, 1);

    struct sockaddr_in stAddr;
    uv_ip4_addr(strIp.c_str(), uPort, &stAddr);
    return Send(pData, iLen, (const struct sockaddr*)&stAddr);
}

int CUvUdp::Send(char* pData, ssize_t iLen, const struct sockaddr* pAddr) {
    ASSERT_RET_VALUE(NULL != mpUdp && NULL != mpUvLoop && NULL != pAddr, 1);
    tagUvUdpPkg stTmp;
    stTmp.stBuf.base = (char*)MemMalloc(iLen);
    stTmp.stBuf.len = (unsigned long)iLen;
    memcpy(stTmp.stBuf.base, pData, iLen);
    memcpy(&stTmp.stAddr, pAddr, sizeof(stTmp.stAddr));

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

void CUvUdp::SendCb(uv_udp_send_t* pReq, int iStatus) {
    if (iStatus < 0) {
        LOG_ERR("Udp Send Error");
    }

    CUvUdp* pUdp = (CUvUdp*)uv_handle_get_data((uv_handle_t*)pReq);
    if (NULL != pUdp) {
        std::map<uv_udp_send_t*, tagUvBufArray>::iterator iter = pUdp->mmapSend.find(pReq);
        if (iter != pUdp->mmapSend.end()) {
            uv_udp_send_t* pWriteReq = iter->first;
			pUdp->MemFree(pWriteReq);
            for (unsigned int i = 0; i < iter->second.iBufNum; ++i) {
				pUdp->MemFree(iter->second.pBufs[i].base);
            }

			pUdp->MemFree(iter->second.pBufs);
            pUdp->mmapSend.erase(iter);
        }
        else {
            LOG_ERR("Can Not Find The WriteReq");
        }

        pUdp->OnSend(iStatus);
        pUdp->DoSend();
    }
}

int CUvUdp::DoSend() {
    if (uv_is_closing((uv_handle_t*)&mpUdp)) {
        return 0;
    }

    tagUvBufArray stBufArray;
    tagUvUdpPkg stTmp;
    mcSendMutex.Lock();
    stBufArray.iBufNum = (unsigned int)mqueSendBuf.size();
    if (stBufArray.iBufNum > 0) {
        stBufArray.pBufs = (uv_buf_t*)MemMalloc(sizeof(uv_buf_t) * stBufArray.iBufNum);
        for (unsigned int i = 0; i < stBufArray.iBufNum; ++i) {
            stTmp = mqueSendBuf.front();
            mqueSendBuf.pop();
            stBufArray.pBufs[i].base = stTmp.stBuf.base;
            stBufArray.pBufs[i].len = stTmp.stBuf.len;
        }
    }
    mcSendMutex.UnLock();

    if (!stBufArray.iBufNum) {
        return 1;
    }

    uv_udp_send_t* pWriteReq = (uv_udp_send_t*)MemMalloc(sizeof(uv_udp_send_t));
    uv_handle_set_data((uv_handle_t*)pWriteReq, (void*)this);
    mmapSend.insert(std::make_pair(pWriteReq, stBufArray));

    return uv_udp_send(pWriteReq, mpUdp, stBufArray.pBufs, stBufArray.iBufNum, (struct sockaddr*)&stTmp.stAddr, CUvUdp::SendCb);
}

int CUvUdp::Start() {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpUdp, 1);
    uv_udp_init(mpUvLoop, mpUdp);
    uv_handle_set_data((uv_handle_t*)mpUdp, (void*)this);
    if (musPort > 0) {
        int iRet = uv_udp_bind(mpUdp, (struct sockaddr*)&mstLocalAddr, UV_UDP_REUSEADDR);
        if (iRet) {
            LOG_ERR("uv_udp_bind error:%s %s,port(%d)", uv_strerror(iRet), uv_err_name(iRet), mstLocalAddr.sin_port);
            return 1;
        }
    }
    else {
        uv_udp_set_broadcast(mpUdp, 1);
    }

    int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
    uv_recv_buffer_size((uv_handle_t*)mpUdp, &iSockBufLen);
    uv_udp_recv_start(mpUdp, CUvBase::UvBufAlloc, CUvUdp::RecvCb);

	mcSendAsyncMutex.Lock();
    uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
    uv_async_init(mpUvLoop, &mstUvSendAsync, CUvUdp::NotifySend);
	mcSendAsyncMutex.UnLock();

    return DoSend();
}

void CUvUdp::CleanSendQueue() {
    mcSendMutex.Lock();
    while (!mqueSendBuf.empty()) {
        tagUvUdpPkg stTmp = mqueSendBuf.front();
        MemFree(stTmp.stBuf.base);
        mqueSendBuf.pop();
    }
    mcSendMutex.UnLock();

    while (!mmapSend.empty()) {
        std::map<uv_udp_send_t*, tagUvBufArray>::iterator iter = mmapSend.begin();
        uv_udp_send_t* pWriteReq = iter->first;
        MemFree(pWriteReq);
        for (unsigned int i = 0; i < iter->second.iBufNum; ++i) {
            MemFree(iter->second.pBufs[i].base);
        }

        MemFree(iter->second.pBufs);
        mmapSend.erase(iter);
    }
}

int CUvUdp::Close() {
    ASSERT_RET_VALUE(NULL != mpUvLoop && NULL != mpUdp && !uv_is_closing((uv_handle_t*)mpUdp), 1);
	mcSendAsyncMutex.Lock();
	if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
		uv_close((uv_handle_t*)&mstUvSendAsync, NULL);
	}
	mcSendAsyncMutex.UnLock();

	uv_close((uv_handle_t*)mpUdp, NULL);
	CleanSendQueue();
    return 0;
}