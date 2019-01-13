#ifndef __CRCOBJECT__H_
#define __CRCOBJECT__H_
#include "CLogmanager.h"
class CRcObject
{
public:
	CRcObject();
	virtual ~CRcObject();

public:
	void IncRef();
	long DecRef();

protected:
	long mlReferCount;
	CMutex mcRefMutex;
};

#define REF(X) \
do{\
if(NULL != (X))	\
{	\
(X)->IncRef();	\
}	\
}while(0)

#define UNREF(X) \
do{\
if (NULL != (X) && (X)->DecRef() <= 0)	\
{	\
DODELETE(X);	\
X = NULL;	\
}\
}while(0)
#endif
