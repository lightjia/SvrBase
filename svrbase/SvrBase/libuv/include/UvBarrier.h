#ifndef __CUVBARRIER__H_
#define __CUVBARRIER__H_
#include "uv.h"
class CUvBarrier {
public:
	CUvBarrier();
	~CUvBarrier();

public:
	int Init(unsigned int iCount);
	int Wait();

private:
	uv_barrier_t mstBarrier;
	bool mbInit;
};

#endif