#ifndef __CCONFIG__H_
#define __CCONFIG__H_
#include "util.h"
#include "rapidjsoninc.h"
#include "confdef.h"
#include "MemOper.h"
class CConfig : public CMemOper{
public:
	CConfig();
	virtual ~CConfig();
	int LoadConfig(const char* pFileName, conf_file_type iType);

public:
	virtual int Init() = 0;
	virtual int UnInit() = 0;

protected:
	virtual int ParseJson(rapidjson::Document& doc) = 0;

private:
	int LoadJsonConf(const char* pData);

protected:
	bool m_bLoad;
};

#endif