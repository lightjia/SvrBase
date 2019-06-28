#include "UvBarrier.h"

CUvBarrier::CUvBarrier() {
	mbInit = false;
}

CUvBarrier::~CUvBarrier() {
	if (mbInit) {
		uv_barrier_destroy(&mstBarrier);
	}
}

int CUvBarrier::Init(unsigned int iCount) {
	mbInit = true;
	return uv_barrier_init(&mstBarrier, iCount);
}

int CUvBarrier::Wait() {
	return uv_barrier_wait(&mstBarrier);
}