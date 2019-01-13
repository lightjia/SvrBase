#include "JsonReadHandler.h"

CJsonReadHandler::CJsonReadHandler(){
}

CJsonReadHandler::~CJsonReadHandler(){
}

int CJsonReadHandler::StartParseJson(const char* pData) {
    ASSERT_RET_VALUE(NULL != pData && strlen(pData) > 0, 1);
    rapidjson::Reader reader;
    rapidjson::StringStream ss(pData);
    reader.IterativeParseInit();
    while (!reader.IterativeParseComplete()) {
        reader.IterativeParseNext<rapidjson::kParseDefaultFlags>(ss, *this);
    }

    return 0;
}