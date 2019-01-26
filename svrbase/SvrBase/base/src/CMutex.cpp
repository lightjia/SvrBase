#include "CMutex.h"

CMutex::CMutex()
{
#if (defined PLATFORM_WINDOWS)
	InitializeCriticalSection(&mCs);
#elif  (defined PLATFORM_LINUX)
	pthread_mutex_init(&mmutex, NULL);
#endif
}

CMutex::~CMutex()
{
#if (defined PLATFORM_WINDOWS)
	DeleteCriticalSection(&mCs);
#elif  (defined PLATFORM_LINUX)
	pthread_mutex_destroy(&mmutex);
#endif
}

void CMutex::Lock()
{
#if (defined PLATFORM_WINDOWS)
	EnterCriticalSection(&mCs);
#elif  (defined PLATFORM_LINUX)
	pthread_mutex_lock(&mmutex);
#endif
}

void CMutex::UnLock()
{
#if (defined PLATFORM_WINDOWS)
	LeaveCriticalSection(&mCs);
#elif  (defined PLATFORM_LINUX)
	pthread_mutex_unlock(&mmutex);
#endif
}