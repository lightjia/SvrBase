#include "UvCond.h"

CUvCond::CUvCond() {
	uv_cond_init(&mstCond);
}

CUvCond::~CUvCond() {
	uv_cond_destroy(&mstCond);
}

void CUvCond::Signal() {
	uv_cond_signal(&mstCond);
}

void  CUvCond::BroadCast() {
	uv_cond_broadcast(&mstCond);
}

void CUvCond::Wait() {
	mcMutex.Lock();
	uv_cond_wait(&mstCond, mcMutex.GetMutex());
	mcMutex.UnLock();
}

int  CUvCond::TimedWait(uint64_t iTimeOut) {
	int iRet = 0;
	mcMutex.Lock();
	iRet = uv_cond_timedwait(&mstCond, mcMutex.GetMutex(), iTimeOut);
	mcMutex.UnLock();

	return iRet;
}