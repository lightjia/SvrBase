#ifndef __CGPUPARAMCONF__H_
#define __CGPUPARAMCONF__H_
#include "Config.h"
#include "singleton.h"
#include <vector>

struct tagGpuConf {
    int iGpuID;
    int iImgWidth;
    int iImgHeight;
    int iListLen;
};

class CGpuParamConf : public CConfig, public CSingleton<CGpuParamConf>
{
    SINGLE_CLASS_INITIAL(CGpuParamConf);

public:
    ~CGpuParamConf();

public:
    int Init();
    int UnInit();

protected:
    int ParseJson(rapidjson::Document& doc);

public:
    std::vector<tagGpuConf> mvecGpuConf;
};

#define sGpuParamConf CGpuParamConf::Instance()
#endif