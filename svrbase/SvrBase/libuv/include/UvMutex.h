#ifndef __CUVMUTEX__H_
#define __CUVMUTEX__H_
#include "uv.h"
class CUvMutex{
public:
    CUvMutex();
    ~CUvMutex();

public:
    void Lock();
    int TryLock();
    void UnLock();
	uv_mutex_t* GetMutex();

private:
    uv_mutex_t mstUvMutex;
};

class CUvAutoMutex {
public:
    CUvAutoMutex(CUvMutex* pMutex);
    ~CUvAutoMutex();

private:
    CUvAutoMutex();

private:
    CUvMutex* mpMutex;
};
#endif