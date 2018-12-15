#ifndef __CIDLE__H_
#define __CIDLE__H_
#include "common.h"
class CIdle
{
public:
	CIdle();
	~CIdle();

public:
	void Sleep(unsigned long iTime = 500);
	void Activate();
private:
#if (defined PLATFORM_WINDOWS)
	HANDLE mHandle;
#elif  (defined PLATFORM_LINUX)
	pthread_cond_t mhCondition;
#endif
};

#endif