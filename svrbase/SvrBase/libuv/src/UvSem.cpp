#include "UvSem.h"

CUvSem::CUvSem(){
    uv_sem_init(&mstUvSem, 0);
}

CUvSem::~CUvSem(){
    uv_sem_destroy(&mstUvSem);
}

void CUvSem::Post() {
    uv_sem_post(&mstUvSem);
}

void CUvSem::Wait() {
    uv_sem_wait(&mstUvSem);
}

int CUvSem::TryWait() {
    return uv_sem_trywait(&mstUvSem);
}