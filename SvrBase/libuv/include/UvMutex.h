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

private:
    uv_mutex_t mstUvMutex;
};

#endif