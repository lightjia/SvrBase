#include "UvNetBase.h"

CUvNetBase::CUvNetBase()
{
    mstrIp = "";
    musPort = 0;
    BZERO(mstLocalAddr);
}

CUvNetBase::~CUvNetBase()
{
}

int CUvNetBase::SetNetParam(const char* pIp, unsigned short iPort) {
    if (NULL != pIp && strlen(pIp) > 0) {
        mstrIp = pIp;
    }
   
    musPort = iPort; 
    return uv_ip4_addr(mstrIp.c_str(), iPort, &mstLocalAddr);
}