#ifndef __UVDEFINE__H_
#define __UVDEFINE__H_
#include "uv.h"

#define UV_DEFAULT_BUF_SIZE 10240   //default 10KB buffer size
#pragma pack(1)
struct tagUvBuf {
    char* pBuf;
    ssize_t iLen;
    ssize_t iUse;
    ssize_t iTotal;
};

struct tagUvBufArray {
    uv_buf_t* pBufs;
    unsigned int iBufNum;
};
#pragma pack()
#endif