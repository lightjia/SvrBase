#ifndef __CUVSEM__H_
#define __CUVSEM__H_
#include "uv.h"
class CUvSem{
public:
    CUvSem();
    ~CUvSem();

public:
    void Post();
    void Wait();
    int TryWait();

private:
    uv_sem_t mstUvSem;
};

#endif