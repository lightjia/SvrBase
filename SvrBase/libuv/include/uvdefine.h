#ifndef __UVDEFINE__H_
#define __UVDEFINE__H_
#include "uv.h"

#define UV_DEFAULT_BUF_SIZE 10240   //default 10KB buffer size
struct tagUvBuf {
    char* pBuf;
    ssize_t iLen;
    ssize_t iUse;
    ssize_t iTotal;
};
#endif