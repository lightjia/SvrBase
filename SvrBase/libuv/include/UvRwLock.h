#ifndef __CUVRWLOCK__H_
#define __CUVRWLOCK__H_
#include "uv.h"
class CUvRwLock
{
public:
	CUvRwLock();
	~CUvRwLock();

public:
	void RdLock();
	void RdUnLock();
	int RdTryLock();

	void WrLock();
	void WrUnLock();
	int WrTryLock();

private:
	uv_rwlock_t mstRwLock;
};

#endif