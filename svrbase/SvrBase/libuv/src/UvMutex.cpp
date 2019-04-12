#include "UvMutex.h"

CUvMutex::CUvMutex(bool bRecursive){
    if (bRecursive) {
        uv_mutex_init_recursive(&mstUvMutex);
    } else {
        uv_mutex_init(&mstUvMutex);
    }
}

CUvMutex::~CUvMutex(){
    uv_mutex_destroy(&mstUvMutex);
}

void CUvMutex::Lock() {
    uv_mutex_lock(&mstUvMutex);
}

void CUvMutex::UnLock() {
    uv_mutex_unlock(&mstUvMutex);
}

int CUvMutex::TryLock() {
    return uv_mutex_trylock(&mstUvMutex);
}

uv_mutex_t* CUvMutex::GetMutex() {
	return &mstUvMutex;
}