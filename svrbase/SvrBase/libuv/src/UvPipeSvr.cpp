#include "UvPipeSvr.h"

CUvPipeSvr::CUvPipeSvr(){
    mpPipeSvr = NULL;
}

CUvPipeSvr::~CUvPipeSvr(){
}

uv_pipe_t* CUvPipeSvr::AllocPipeCli() {
    uv_pipe_t* pPipeCli = (uv_pipe_t*)MemMalloc(sizeof(uv_pipe_t));
    //Can Use Another Loop In SubClass
    uv_pipe_init(GetUvLoop(), pPipeCli, miIpc);
    return pPipeCli;
}

int CUvPipeSvr::FreePipeCli(uv_pipe_t* pPipeCli) {
	ASSERT_RET_VALUE(pPipeCli && mpUvLoop, 1);
	uv_close((uv_handle_t*)pPipeCli, NULL);
	MemFree(pPipeCli);

	return 0;
}

void CUvPipeSvr::DoConn(int iStatus) {
	if (iStatus) {
		LOG_ERR("uv_conn error:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
		return;
	}

	uv_pipe_t* pPipeCli = AllocPipeCli();
	ASSERT_RET(NULL != pPipeCli);
	int iRet = uv_accept((uv_stream_t*)mpPipeSvr, (uv_stream_t*)pPipeCli);
	if (iRet) {
		LOG_ERR("uv_accept error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
		FreePipeCli(pPipeCli);
		return;
	}

	if (OnAccept(pPipeCli) != 0) {
		FreePipeCli(pPipeCli);
	}
}

void CUvPipeSvr::ConnCb(uv_stream_t* pHandle, int iStatus) {
    CUvPipeSvr* pTcpSvr = (CUvPipeSvr*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (NULL != pTcpSvr) {
        if (iStatus) {
            LOG_ERR("uv_conn error:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
            return;
        }

        uv_pipe_t* pPipeCli = pTcpSvr->AllocPipeCli();
        ASSERT_RET(NULL != pPipeCli);
        int iRet = uv_accept((uv_stream_t*)pTcpSvr->mpPipeSvr, (uv_stream_t*)pPipeCli);
        if (iRet) {
            LOG_ERR("uv_accept error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
            return;
        }

        if (pTcpSvr->OnAccept(pPipeCli) != 0) {
            DOFREE(pPipeCli);
        }
    }
}

int CUvPipeSvr::Listen(int iBackLog) {
	int iRet = 1;
    ASSERT_RET_VALUE(mpUvLoop && mpPipeSvr, iRet);
	/*uv_fs_t stUvFsReq;
	uv_fs_unlink(GetUvLoop(), &stUvFsReq, mstrPipeName.c_str(), NULL);*/

	uv_handle_set_data((uv_handle_t*)mpPipeSvr, (void*)this);
	uv_pipe_init(GetUvLoop(), mpPipeSvr, miIpc);
	iRet = uv_pipe_bind(mpPipeSvr, mstrPipeName.c_str());
	if (iRet) {
		LOG_ERR("uv_pipe_bind error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
		uv_close((uv_handle_t*)mpPipeSvr, NULL);
		return iRet;
	}

	iRet = uv_listen((uv_stream_t*)mpPipeSvr, iBackLog, CUvPipeSvr::ConnCb);
	if (iRet) {
		LOG_ERR("uv_listen error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
		uv_close((uv_handle_t*)mpPipeSvr, NULL);
	}

	return iRet;
}