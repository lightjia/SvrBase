#ifndef __CUVCOND__H_
#define __CUVCOND__H_
#include "UvMutex.h"
class CUvCond
{
public:
	CUvCond();
	~CUvCond();

public:
	void Signal();
	void BroadCast();
	void Wait();
	int TimedWait(uint64_t iTimeOut);

private:
	CUvMutex mcMutex;
	uv_cond_t mstCond;
};

#endif