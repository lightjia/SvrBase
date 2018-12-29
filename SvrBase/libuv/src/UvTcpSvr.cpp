#include "UvTcpSvr.h"

CUvTcpSvr::CUvTcpSvr(){
    mpTcpSvr = nullptr;
}

CUvTcpSvr::~CUvTcpSvr(){
}

uv_tcp_t* CUvTcpSvr::AllocTcpCli() {
    uv_tcp_t* pTcpCli = (uv_tcp_t*)do_malloc(sizeof(uv_tcp_t));
    //Can Use Another Loop In SubClass
    uv_tcp_init(GetUvLoop(), pTcpCli);
    return pTcpCli;
}

void CUvTcpSvr::ConnCb(uv_stream_t* pHandle, int iStatus){
    CUvTcpSvr* pTcpSvr = (CUvTcpSvr*)uv_handle_get_data((uv_handle_t*)pHandle);
    if (nullptr != pTcpSvr){
        if (iStatus){
            LOG_ERR("uv_conn error:%s %s", uv_strerror(iStatus), uv_err_name(iStatus));
            return;
        }

        uv_tcp_t* pTcpCli = pTcpSvr->AllocTcpCli();
        ASSERT_RET(nullptr != pTcpCli);
        int iRet = uv_accept((uv_stream_t*)pTcpSvr->mpTcpSvr, (uv_stream_t*)pTcpCli);
        if (iRet){
            LOG_ERR("uv_accept error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
            return;
        }

        if (pTcpSvr->OnAccept(pTcpCli) != 0){
            DOFREE(pTcpCli);
        }
    }
}

int CUvTcpSvr::Listen(int iBackLog){
    if (nullptr == mpUvLoop || nullptr == mpTcpSvr){
        return 1;
    }

    uv_handle_set_data((uv_handle_t*)mpTcpSvr, (void*)this);
    uv_tcp_init(mpUvLoop, mpTcpSvr);
    int iRet = uv_tcp_bind(mpTcpSvr, (struct sockaddr*)&mstLocalAddr, SO_REUSEADDR);
    if (iRet){
        LOG_ERR("uv_tcp_bind error:%s %s", uv_strerror(iRet), uv_err_name(iRet));
        return 1;
    }

    return uv_listen((uv_stream_t*)mpTcpSvr, iBackLog, CUvTcpSvr::ConnCb);
}