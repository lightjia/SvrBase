#ifndef __CGUIDTOOL__H_
#define __CGUIDTOOL__H_
#include "singleton.h"
#if (defined PLATFORM_LINUX)
#include "uuid/uuid.h"
#endif
#include <string>

class CGuidTool : public CSingleton<CGuidTool>
{
    SINGLE_CLASS_INITIAL(CGuidTool);

public:
    ~CGuidTool();

public:
    std::string CreateGuid();
};
#define sGuidTool CGuidTool::Instance()
#endif