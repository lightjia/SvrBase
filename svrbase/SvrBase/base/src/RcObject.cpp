
#include "RcObject.h"

CRcObject::CRcObject()
{
	mlReferCount = 0;
}

CRcObject::~CRcObject()
{
}

void CRcObject::IncRef()
{
	mcRefMutex.Lock();
	//atomic_change(&mlReferCount, (int)1);
	++mlReferCount;
	mcRefMutex.UnLock();
}

long CRcObject::DecRef()
{
	long lRet = 1;
	mcRefMutex.Lock();
	//atomic_change(&mlReferCount, (int)-1);
	--mlReferCount;
	lRet = mlReferCount;
	mcRefMutex.UnLock();

	return lRet;
}