#include "ConfMgr.h"

CConfMgr::CConfMgr()
{
	m_bInit = false;
}

CConfMgr::~CConfMgr()
{
}

int CConfMgr::Init()
{
	if (!m_bInit)
	{
		sSvrParamConf->Init();
        sGpuParamConf->Init();
		m_bInit = true;
	}
	return 0;
}

int CConfMgr::UnInit()
{
	if (m_bInit)
	{
		sSvrParamConf->UnInit();
        sGpuParamConf->UnInit();
		m_bInit = false;
	}
	
	return 0;
}