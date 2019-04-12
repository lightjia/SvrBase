#ifndef __CMUTEX__H_
#define __CMUTEX__H_
#include "common.h"

class CMutex{
public:
	virtual void Lock() = 0;
	virtual int TryLock() = 0;
	virtual void UnLock() = 0;
};

class CAutoMutex {
public:
	CAutoMutex(CMutex* pMutex) {
		mpMutex = pMutex; 
		if (mpMutex) {
			mpMutex->Lock();
		}
	}

	~CAutoMutex() {
		if (mpMutex) {
			mpMutex->UnLock();
		}
	}

private:
	CAutoMutex();

private:
	CMutex* mpMutex;
};
#endif