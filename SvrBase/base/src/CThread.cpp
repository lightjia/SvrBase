#include "CThread.h"
#include "CLogmanager.h"
CThread::CThread()
{
	mdwThreadID = 0;
	mbStop = false;
	mbExitMainFunc = false;
	miThreadState = THREAD_STATE_INIT;
	miType = THREAD_TYPE_CONTROL;
}

CThread::~CThread()
{
}

void CThread::Wait(unsigned long iTime)
{
  if (miThreadState == THREAD_STATE_RUN)
  {
    miThreadState = THREAD_STATE_SLEEP;
    mcIdle.Sleep(iTime);
    miThreadState = THREAD_STATE_RUN;
  }
}

void CThread::Activate()
{
	if (miThreadState == THREAD_STATE_SLEEP)
	{
		mcIdle.Activate();
		miThreadState = THREAD_STATE_RUN;
	}
}

void* CThread::ThreadEntry(void* pParam)
{
	void* pRet = nullptr;

	CThread* pThread = reinterpret_cast<CThread*>(pParam);
	if (nullptr != pThread)
	{
    pThread->miThreadState = THREAD_STATE_RUN;
		pThread->mdwThreadID = get_thread_id_self();
		pRet = pThread->work();
		pThread->mbExitMainFunc = true;
#if (defined PLATFORM_WINDOWS)
		_endthread();	//自动回收资源
#elif  (defined PLATFORM_LINUX)
		pthread_detach(pthread_self());	//自动回收资源
#endif
	}

	return pRet;
}

int CThread::Start(bool bWaitCreateSuccesss)
{
#if (defined PLATFORM_WINDOWS)
	if(_beginthread((void(__cdecl *) (void *))CThread::ThreadEntry, 0, (void*)this) == -1)
	{
		return 1;
	}
#elif  (defined PLATFORM_LINUX)
	if (::pthread_create((pthread_t*)&mdwThreadID, nullptr, CThread::ThreadEntry, (void*)this) != 0)
	{
		return 1;
	}
#endif

  while (bWaitCreateSuccesss && miThreadState != THREAD_STATE_RUN)
  {
    mcIdle.Sleep();
  }

	return 0;
}

int CThread::Stop()
{
	mbStop = true;
	Activate();
	while (!mbExitMainFunc)
	{
		Wait();
	}

	return AfterStop();
}

int CThread::AfterStop()
{
	miThreadState = THREAD_STATE_EXIT;
	mdwThreadID = 0;
	return 0;
}