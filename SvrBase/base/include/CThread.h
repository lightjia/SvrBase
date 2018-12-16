#ifndef __CTHREAD__H_
#define __CTHREAD__H_
#include "common.h"
#include "CMutex.h"
#include "Idle.h"
#include "LocalDefine.h"

enum THREADSTATE
{
	THREAD_STATE_INIT = 1,	//�̳߳�ʼ��״̬
	THREAD_STATE_RUN,	//�߳�����״̬
	THREAD_STATE_SLEEP,	//�߳�����״̬
	THREAD_STATE_WAIT_EXIT,	//�̵߳ȴ��˳�״̬
	THREAD_STATE_EXIT,	//�߳��Ѿ��˳�
};

enum THREADTYPE
{
	THREAD_TYPE_CONTROL	= 0,	//�̱߳�����
	THREAD_TYPE_AUTO_EXIT,	//�߳��Զ��˳�
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
	virtual void* work() = 0;	//�����߳�ִ�к���

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