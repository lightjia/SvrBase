#include "UvRwLock.h"

CUvRwLock::CUvRwLock() {
	uv_rwlock_init(&mstRwLock);
}

CUvRwLock::~CUvRwLock() {
	uv_rwlock_destroy(&mstRwLock);
}

void CUvRwLock::RdLock() {
	uv_rwlock_rdlock(&mstRwLock);
}

void CUvRwLock::RdUnLock() {
	uv_rwlock_rdunlock(&mstRwLock);
}

int CUvRwLock::RdTryLock() {
	return uv_rwlock_tryrdlock(&mstRwLock);
}

void CUvRwLock::WrLock() {
	uv_rwlock_wrlock(&mstRwLock);
}

void CUvRwLock::WrUnLock() {
	uv_rwlock_wrunlock(&mstRwLock);
}

int CUvRwLock::WrTryLock() {
	return uv_rwlock_trywrlock(&mstRwLock);
}