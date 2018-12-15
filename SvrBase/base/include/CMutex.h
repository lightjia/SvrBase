#ifndef __CMUTEX__H_
#define __CMUTEX__H_
#include "common.h"

class CMutex
{
public:
	CMutex();
	~CMutex();
	void Lock();
	void UnLock();
public:
#if (defined PLATFORM_WINDOWS)
#elif  (defined PLATFORM_LINUX)
	pthread_mutex_t* GetMutex() {
		return &mmutex;
	}
#endif
	
private:
#if (defined PLATFORM_WINDOWS)
	CRITICAL_SECTION mCs;
#elif  (defined PLATFORM_LINUX)
	pthread_mutex_t mmutex;
#endif
};

#endif