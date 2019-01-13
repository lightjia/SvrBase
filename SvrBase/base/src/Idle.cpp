#include "Idle.h"
#include "CMutex.h"

CIdle::CIdle()
{
#if (defined PLATFORM_WINDOWS)
	mHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
#elif  (defined PLATFORM_LINUX)
	pthread_condattr_t cond_attr;
	pthread_condattr_init(&cond_attr);
	pthread_cond_init(&mhCondition, &cond_attr);
	pthread_condattr_destroy(&cond_attr);
#endif
}

CIdle::~CIdle()
{
#if (defined PLATFORM_WINDOWS)
	if (NULL != mHandle)
	{
		CloseHandle(mHandle);
	}
#elif  (defined PLATFORM_LINUX)
	pthread_cond_destroy(&mhCondition);
#endif
}

void CIdle::Sleep(unsigned long iTime)
{
#if (defined PLATFORM_WINDOWS)
	if (NULL != mHandle)
	{
		WaitForSingleObject(mHandle, iTime);
	}
#elif  (defined PLATFORM_LINUX)
	if(mhCondition.__data.__total_seq != -1ULL)
	{
		CMutex cTemp;
		cTemp.Lock();
		timespec ts;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		int64_t usec = tv.tv_usec + iTime * 1000LL;
		ts.tv_sec = tv.tv_sec + usec / 1000000;
		ts.tv_nsec = (usec % 1000000) * 1000;
		pthread_cond_timedwait(&mhCondition, cTemp.GetMutex(), &ts);
		cTemp.UnLock();
	}
#endif
}

void CIdle::Activate()
{
#if (defined PLATFORM_WINDOWS)
	if (NULL != mHandle)
	{
		SetEvent(mHandle);
	}
#elif  (defined PLATFORM_LINUX)
	if (mhCondition.__data.__total_seq != -1ULL)
	{
		pthread_cond_signal(&mhCondition);
	}
#endif
}