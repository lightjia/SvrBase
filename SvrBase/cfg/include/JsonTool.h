#ifndef __CJSONTOOL__H_
#define __CJSONTOOL__H_
#include "singleton.h"
#include "rapidjsoninc.h"
#include <map>

class CJsonTool : public CSingleton<CJsonTool>
{
	SINGLE_CLASS_INITIAL(CJsonTool);
public:
	~CJsonTool();

public:
	int DecodeJsonArray(const char* pJson, std::map<std::string, std::string>& mapJson);
	int  EncodeJsonArray(std::string& strRet, std::map<std::string, std::string>& mapJson);
};
#define sJsonTool CJsonTool::Instance()
#endif