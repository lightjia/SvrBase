#ifndef __CEVENTMGR__H_
#define __CEVENTMGR__H_
#include "singleton.h"
#include "CMutex.h"
#include "CLogmanager.h"
#include <string>
#include <map>
#include <set>

class CEventListener {
public:
    virtual int OnEvent(void* pParam) = 0;
};

class CEventMgr : public CSingleton<CEventMgr>{
    SINGLE_CLASS_INITIAL(CEventMgr);
public:
    ~CEventMgr();

public:
    int AddListener(std::string strListenName, CEventListener* pListener);
    int DelListener(CEventListener* pListener);
    int DispatchEvent(std::string strListenName, void* pParam);

private:
    std::map<std::string, std::set<CEventListener*>> mmapEventListeners;
    CMutex mcMapEventListenersMutex;
};

#endif