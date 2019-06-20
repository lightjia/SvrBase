#include "UvPipeCli.h"
enum UV_PIPE_CLI_STATE {
	UV_PIPE_CLI_STATE_NONE,
	UV_PIPE_CLI_STATE_ESTAB,
	UV_PIPE_CLI_STATE_CLOSE,
};

CUvPipeCli::CUvPipeCli(){
	mpPipeCli = NULL;
	mpUvConn = NULL;
	mpSendBuf = NULL;
	miTotalSendBytes = 0;
	miNeedSendBytes = 0;
	miTotalRecvBytes = 0;
	miTcpCliState = UV_PIPE_CLI_STATE_NONE;
}

CUvPipeCli::~CUvPipeCli(){
    CleanSendQueue();
}

void CUvPipeCli::DoRecv(ssize_t nRead, const uv_buf_t* pBuf) {
	if (nRead == 0) {
		LOG_DBG("%s recv 0 bytes", GetPipeId().c_str());
	} else if (nRead < 0) {
		Close();
	} else if (nRead > 0) {
		miTotalRecvBytes += (unsigned long)nRead;
		OnRecv(nRead, pBuf);
	}
}

void CUvPipeCli::RecvCb(uv_stream_t* pHandle, ssize_t nRead, const uv_buf_t* pBuf) {
	CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pHandle);
	ASSERT_RET(pPipeCli);
	pPipeCli->DoRecv(nRead, pBuf);
}

void CUvPipeCli::SetPipeCli(uv_pipe_t* pPipeCli) {
	ASSERT_RET(NULL != mpUvLoop);
	mpPipeCli = pPipeCli;
	uv_handle_set_data((uv_handle_t*)mpPipeCli, (void*)this);
	AfterConn();
}

int CUvPipeCli::AfterConn() {
	StartRecv();

	mcSendMutex.Lock();
	uv_handle_set_data((uv_handle_t*)&mstUvSendAsync, (void*)this);
	uv_async_init(mpUvLoop, &mstUvSendAsync, CUvPipeCli::NotifySend);
	miTcpCliState = UV_PIPE_CLI_STATE_ESTAB;
	mcSendMutex.UnLock();

	return OnConn(0);
}

void CUvPipeCli::DoConn(int iStatus) {
	if (!iStatus) {
		AfterConn();
		DoSend();
	} else {
		LOG_ERR("uv_tcp_connect error:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
		OnConn(iStatus);
		uv_close((uv_handle_t*)mpPipeCli, NULL);
	}
}

void CUvPipeCli::ConnCb(uv_connect_t* pReq, int iStatus) {
	CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pReq);
	ASSERT_RET(pPipeCli);
	pPipeCli->DoConn(iStatus);
}

int CUvPipeCli::Connect() {
	int iRet = 1;
	ASSERT_RET_VALUE(mpPipeCli && mpUvLoop && mpUvConn, iRet);

	uv_handle_set_data((uv_handle_t*)mpPipeCli, (void*)this);
	iRet = uv_pipe_init(GetUvLoop(), mpPipeCli, miIpc);
	if (iRet) {
		LOG_ERR("uv_pipe_init error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
	}

	uv_handle_set_data((uv_handle_t*)mpUvConn, (void*)this);
	uv_pipe_connect(mpUvConn, mpPipeCli, mstrPipeName.c_str(), CUvPipeCli::ConnCb);

	return iRet;
}

int CUvPipeCli::StartRecv() {
	ASSERT_RET_VALUE(mpPipeCli && mpUvLoop, 1);

	int iSockBufLen = (int)(mstUvBuf.iLen - mstUvBuf.iUse);
	uv_recv_buffer_size((uv_handle_t*)mpPipeCli, &iSockBufLen);
	return uv_read_start((uv_stream_t*)mpPipeCli, CUvBase::UvBufAlloc, CUvPipeCli::RecvCb);
}

void CUvPipeCli::AfterSend(uv_write_t* pReq, int iStatus) {
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
	}
	else {
		DoSend();
	}
}

void CUvPipeCli::SendCb(uv_write_t* pReq, int iStatus) {
	CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pReq);
	ASSERT_RET(pPipeCli);
	pPipeCli->AfterSend(pReq, iStatus);
}

void CUvPipeCli::NotifySend(uv_async_t* pHandle) {
	CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pHandle);
	if (NULL != pPipeCli) {
		pPipeCli->DoSend();
	}
}

int CUvPipeCli::DoSend() {
	CAutoMutex cAutoMutex(&mcSendMutex);
	if (UV_PIPE_CLI_STATE_ESTAB == miTcpCliState) {
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
			int iWriteRet = uv_write(pWriteReq, (uv_stream_t*)mpPipeCli, stBufArray.pBufs, stBufArray.iBufNum, CUvPipeCli::SendCb);
			if (iWriteRet) {
				LOG_ERR("uv_write:%s %s", uv_strerror(iWriteRet), uv_err_name(iWriteRet));
			}
		}
	}

	return 0;
}

int CUvPipeCli::Send(char* pData, ssize_t iLen) {
	ASSERT_RET_VALUE(pData && iLen > 0 && mpPipeCli && mpUvLoop && UV_PIPE_CLI_STATE_CLOSE != miTcpCliState, 1);
	CAutoMutex cAutoMutex(&mcSendMutex);
	if (!mpSendBuf) {
		mpSendBuf = new CMemBuffer();
	}

	ASSERT_RET_VALUE(mpSendBuf, 1);
	mpSendBuf->Append(pData, iLen);
	miNeedSendBytes += iLen;
	if (UV_PIPE_CLI_STATE_ESTAB == miTcpCliState) {
		uv_async_send(&mstUvSendAsync);
	}

	return 0;
}

void CUvPipeCli::CleanSendQueue() {
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

void CUvPipeCli::CloseCb(uv_handle_t* pHandle) {
	CUvPipeCli* pPipeCli = (CUvPipeCli*)uv_handle_get_data((uv_handle_t*)pHandle);
	if (pPipeCli) {
		pPipeCli->OnClose();
		//use uv_is_active to panduan
		uv_close((uv_handle_t*)&pPipeCli->mstUvSendAsync, NULL);
	}
}

int CUvPipeCli::Close() {
	ASSERT_RET_VALUE(mpPipeCli && mpUvLoop, 1);
	CAutoMutex cAutoMutex(&mcSendMutex);
	if (UV_PIPE_CLI_STATE_ESTAB == miTcpCliState) {
		uv_close((uv_handle_t*)mpPipeCli, CUvPipeCli::CloseCb);
		miTcpCliState = UV_PIPE_CLI_STATE_CLOSE;
	}

	return 0;
}