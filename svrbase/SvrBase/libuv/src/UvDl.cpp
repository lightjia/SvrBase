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
	if (uv_dlopen(pFileName, &mstLib)) {
		LOG_ERR("uv_dlopen error: %s", GetErr());
		return 1;
	}

    return 0;
}

int CUvDl::GetFunc(const char* pFuncName, void** pFuncPtr) {
    ASSERT_RET_VALUE(pFuncPtr && pFuncName && strlen(pFuncName), 1);
	if (uv_dlsym(&mstLib, pFuncName, pFuncPtr)) {
		LOG_ERR("uv_dlsym error: %s", GetErr());
	}

    return 0;
}

void CUvDl::Close() {
    uv_dlclose(&mstLib);
}