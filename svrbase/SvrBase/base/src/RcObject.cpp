#include "RcObject.h"

CRcObject::CRcObject(){
	miReferCount = 0;
}

CRcObject::~CRcObject(){
}

void CRcObject::IncRef(){
	mcRefMutex.Lock();
	++miReferCount;
	mcRefMutex.UnLock();
}

int CRcObject::DecRef(){
	int iRet = 1;
	mcRefMutex.Lock();
	--miReferCount;
	iRet = miReferCount;
	mcRefMutex.UnLock();

	return iRet;
}