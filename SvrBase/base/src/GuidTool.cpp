#include "GuidTool.h"

CGuidTool::CGuidTool(){
}

CGuidTool::~CGuidTool(){
}

std::string CGuidTool::CreateGuid() {
#define GUID_LEN 64 
    char buffer[GUID_LEN] = { 0 };
#if (defined PLATFORM_WINDOWS)
    GUID guid;
    if (CoCreateGuid(&guid)){
        fprintf(stderr, "create guid error\n");
    }

    snprintf(buffer, GUID_LEN,
        "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2],
        guid.Data4[3], guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
#elif  (defined PLATFORM_LINUX)
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, buffer);
#endif

    return buffer;
}