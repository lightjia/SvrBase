#ifndef __CCONFMGR__H_
#define __CCONFMGR__H_
#include "singleton.h"
#include "ServerParamConf.h"
#include "GpuParamConf.h"
class CConfMgr : public CSingleton<CConfMgr>
{
	SINGLE_CLASS_INITIAL(CConfMgr);

public:
	~CConfMgr();

public:
	int Init();
	int UnInit();

private:
	bool m_bInit;
};

#define sConfMgr CConfMgr::Instance()
#endif