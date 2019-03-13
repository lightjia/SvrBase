#include "UvDl.h"

CUvDl::CUvDl(){
    BZERO(mstLib);
}

CUvDl::~CUvDl(){
    Close();
}

const char* CUvDl::GetErr() {
    return uv_dlerror(&mstLib);
}

int CUvDl::Open(const char* pFileName) {
    ASSERT_RET_VALUE(pFileName && strlen(pFileName), 1);
    return uv_dlopen(pFileName, &mstLib);
}

int CUvDl::GetFunc(const char* pFuncName, void** pFuncPtr) {
    ASSERT_RET_VALUE(pFuncPtr && pFuncName && strlen(pFuncName), 1);

    return uv_dlsym(&mstLib, pFuncName, pFuncPtr);
}

void CUvDl::Close() {
    uv_dlclose(&mstLib);
}