#ifndef __CUVMUTEX__H_
#define __CUVMUTEX__H_
#include "uv.h"
#include "Mutex.h"
class CUvMutex : public CMutex{
public:
    CUvMutex(bool bRecursive = false);
    ~CUvMutex();

public:
    void Lock();
    int TryLock();
    void UnLock();
	uv_mutex_t* GetMutex();

private:
    uv_mutex_t mstUvMutex;
};
#endif