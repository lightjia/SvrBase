#include "UvPipeCli.h"

CUvPipeCli::CUvPipeCli(){
    mpPipeCli = NULL;
    mpUvConn = NULL;
}

CUvPipeCli::~CUvPipeCli(){
    CleanSendQueue();
}

void CUvPipeCli::RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf) {
    CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pPipeCli) {
        if (nRead == 0) {
            if (uv_is_closing((uv_handle_t*)pPipeCli->mpPipeCli)) {
                LOG_ERR("Cli is Closed!");
            }

            return;
        }

        if (nRead < 0) {
            pPipeCli->Close();
            return;
        }
        else if (nRead > 0) {
            pPipeCli->OnRecv(nRead, pBuf);
        }
    }
}

void CUvPipeCli::SetPipeCli(uv_pipe_t* pPipeCli) {
    ASSERT_RET(NULL != mpUvLoop);
    mpPipeCli = pPipeCli;
    uv_handle_set_data((uv_handle_t*)mpPipeCli, (void*)this);
    AfterConn();
}

int CUvPipeCli::AfterConn() {
    Recv();
    uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
	mcSendAsyncMutex.Lock();
	uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
	uv_async_init(mpUvLoop, &mstUvSendAsync, CUvPipeCli::NotifySend);
	mcSendAsyncMutex.UnLock();

    return 0;
}

void CUvPipeCli::ConnCb(uv_connect_t* pReq, int iStatus) {
    CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pReq);
    if (NULL != pPipeCli) {
        if (iStatus >= 0) {
            pPipeCli->AfterConn();
        }

        pPipeCli->OnConn(iStatus);
        pPipeCli->DoSend();
    }
}

int CUvPipeCli::Connect() {
    ASSERT_RET_VALUE(mpPipeCli && mpUvLoop && mpUvConn, 1);

    uv_handle_set_data((uv_handle_t*)mpPipeCli, (void*)this);
    uv_pipe_init(GetUvLoop(), mpPipeCli, miIpc);

    uv_handle_set_data((uv_handle_t*)mpUvConn, (void*)this);
    uv_pipe_connect(mpUvConn, mpPipeCli, mstrPipeName.c_str(), CUvPipeCli::ConnCb);
    return 0;
}

int CUvPipeCli::Recv() {
    if (NULL == mpPipeCli || NULL == mpUvLoop) {
        return 1;
    }

    int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
    uv_recv_buffer_size((uv_handle_t*)mpPipeCli, &iSockBufLen);
    return uv_read_start((uv_stream_t*)mpPipeCli, CUvBase::UvBufAlloc, CUvPipeCli::RecvCb);
}

void CUvPipeCli::SendCb(uv_write_t* pReq, int iStatus) {
    CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pReq);
    if (NULL != pPipeCli)
    {
        std::map<uv_write_t*, uv_buf_t*>::iterator iter = pPipeCli->mmapSend.find(pReq);
        if (iter != pPipeCli->mmapSend.end()) {
            uv_write_t* pWriteReq = iter->first;
            DOFREE(pWriteReq);
            uv_buf_t* pBuf = iter->second;
            DOFREE(pBuf->base);
            DOFREE(pBuf);
            pPipeCli->mmapSend.erase(iter);
        }
        else {
            LOG_ERR("Can Not Find The WriteReq");
        }

        pPipeCli->OnSend(iStatus);
        if (iStatus) {
            if (iStatus == UV_ECANCELED) {
                return;
            }

			LOG_ERR("uv_write:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
            pPipeCli->Close();
        }
        else
        {
            pPipeCli->DoSend();
        }
    }
}

void CUvPipeCli::NotifySend(uv_async_t* pHandle) {
    CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pPipeCli) {
        pPipeCli->DoSend();
    }
}

int CUvPipeCli::DoSend() {
	if (uv_is_closing((uv_handle_t*)&mpPipeCli)) {
		return 0;
	}

    uv_buf_t* pBuf = NULL;
    mcSendMutex.Lock();
    if (!mqueSendBuf.empty()) {
        uv_buf_t stTmp = mqueSendBuf.front();
        mqueSendBuf.pop();
        pBuf = (uv_buf_t*)do_malloc(sizeof(uv_buf_t));
        pBuf->base = stTmp.base;
        pBuf->len = stTmp.len;
    }
    mcSendMutex.UnLock();

    if (!pBuf) {
        return 1;
    }

    uv_write_t* pWriteReq = (uv_write_t*)do_malloc(sizeof(uv_write_t));
    uv_handle_set_data((uv_handle_t*)pWriteReq, (void*)this);
    mmapSend.insert(std::make_pair(pWriteReq, pBuf));
    return uv_write(pWriteReq, (uv_stream_t*)mpPipeCli, pBuf, 1, CUvPipeCli::SendCb);
}

int CUvPipeCli::Send(char* pData, ssize_t iLen) {
    ASSERT_RET_VALUE(pData && iLen > 0 && mpPipeCli && mpUvLoop && uv_is_active((uv_handle_t*)&mstUvSendAsync), 1);

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

void CUvPipeCli::CleanSendQueue() {
    mcSendMutex.Lock();
    while (!mqueSendBuf.empty()) {
        uv_buf_t stTmp = mqueSendBuf.front();
        DOFREE(stTmp.base);
        mqueSendBuf.pop();
    }
    mcSendMutex.UnLock();

    while (!mmapSend.empty()) {
        std::map<uv_write_t*, uv_buf_t*>::iterator iter = mmapSend.begin();
        uv_write_t* pWriteReq = iter->first;
        DOFREE(pWriteReq);
        uv_buf_t* pBuf = iter->second;
        DOFREE(pBuf->base);
        DOFREE(pBuf);
        mmapSend.erase(iter);
    }
}

void CUvPipeCli::CloseCb(uv_handle_t* pHandle) {
    CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pPipeCli) {
        pPipeCli->OnClose();
    }
}

int CUvPipeCli::Close() {
    ASSERT_RET_VALUE(mpUvLoop && mpPipeCli && !uv_is_closing((uv_handle_t*)mpPipeCli), 1);

	mcSendAsyncMutex.Lock();
	if (uv_is_active((uv_handle_t*)&mstUvSendAsync)) {
		uv_close((uv_handle_t*)&mstUvSendAsync, NULL);
	}
	mcSendAsyncMutex.UnLock();

    uv_close((uv_handle_t*)mpPipeCli, CUvPipeCli::CloseCb);
    CleanSendQueue();
    return 0;
}