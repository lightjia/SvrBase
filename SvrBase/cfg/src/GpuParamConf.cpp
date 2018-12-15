#include "GpuParamConf.h"

#define CONF_FILE_NAME "gpu_param.json"

CGpuParamConf::CGpuParamConf(){
}

CGpuParamConf::~CGpuParamConf(){
}

int CGpuParamConf::Init() {
    if (!m_bLoad && 0 == LoadConfig(CONF_FILE_NAME, CONF_FILE_TYPE_JSON))
    {
        m_bLoad = true;
    }

    return 0;
}

int CGpuParamConf::UnInit() {
    return 0;
}

int CGpuParamConf::ParseJson(rapidjson::Document& doc) {
    if (doc.HasMember("gpu")) {
        rapidjson::Value &gpu = doc["gpu"];
        if (gpu.IsArray()) {
            for (rapidjson::SizeType i = 0; i < gpu.Size(); i++){
                rapidjson::Value &v = gpu[i];
                if (v.HasMember("gpuid") && v.HasMember("imghight") && v.HasMember("imgwidth")){
                    tagGpuConf stTmp;
                    stTmp.iGpuID = v["gpuid"].GetInt(); 
                    stTmp.iImgWidth = v["imgwidth"].GetInt();
                    stTmp.iImgHeight = v["imghight"].GetInt();
                    stTmp.iListLen = v["listlen"].GetInt();
                    mvecGpuConf.push_back(stTmp);
                }
            }
        }
    } else {
        fprintf(stderr, "%s parse error", CONF_FILE_NAME);
    }

    return 0;
}