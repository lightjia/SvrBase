#ifndef __CUVDL__H_
#define __CUVDL__H_
#include "uv.h"
#include "Log.h"
class CUvDl{
public:
    CUvDl();
    ~CUvDl();

public:
    int Open(const char* pFileName);
    void Close();
    int GetFunc(const char* pFuncName, void** pFuncPtr);
    const char* GetErr();

private:
    uv_lib_t mstLib;
};
#endif