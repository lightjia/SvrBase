#include "UvPipeBase.h"

CUvPipeBase::CUvPipeBase(){
    miIpc = 0;
}


CUvPipeBase::~CUvPipeBase(){
}

void CUvPipeBase::SetPipeParam(std::string strPipeName, int iIpc) {
    //win name like "\\\\?\\pipe\\uv-test"
    //linux name like "/tmp/uv-test-sock"

    mstrPipeName = strPipeName;
    miIpc = iIpc;
}