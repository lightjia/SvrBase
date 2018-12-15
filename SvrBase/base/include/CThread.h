#ifndef __CTHREAD__H_
#define __CTHREAD__H_
#include "common.h"
#include "CMutex.h"
#include "Idle.h"

enum THREADSTATE
{
	THREAD_STATE_INIT = 1,	//线程初始化状态
	THREAD_STATE_RUN,	//线程运行状态
	THREAD_STATE_SLEEP,	//线程休眠状态
	THREAD_STATE_WAIT_EXIT,	//线程等待退出状态
	THREAD_STATE_EXIT,	//线程已经退出
};

enum THREADTYPE
{
	THREAD_TYPE_CONTROL	= 0,	//线程被管理
	THREAD_TYPE_AUTO_EXIT,	//线程自动退出
};

class CThread
{
public:
	CThread();
	virtual ~CThread();
	int GetThreadState(){ return miThreadState; }
	void SetType(int iType){ miType = iType; }

public:
	static void* ThreadEntry(void* pParam);

protected:
	virtual void* work() = 0;	//子类线程执行函数

protected:
	//typedef void* (*thread_proc) (void *);
	int Start(bool bWaitCreateSuccesss = true);
	void Wait(unsigned long iTime = 500);
	void Activate();
	int AfterStop();
	int Stop();

protected:
	DWORD mdwThreadID;

protected:
	bool mbStop;
	int miType;
	int miThreadState;
	CMutex mcMutex;
	CIdle mcIdle;
	bool mbExitMainFunc;
};

#endif