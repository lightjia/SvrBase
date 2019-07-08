#include "EventMgr.h"

CEventMgr::CEventMgr() {
}

CEventMgr::~CEventMgr() {
}

int CEventMgr::AddListener(std::string strListenName, CEventListener* pListener) {
	ASSERT_RET_VALUE(NULL != pListener, 1);
	CAutoMutex cAutoMutex(&mcMapEventListenersMutex);
	std::map<std::string, std::set<CEventListener*>>::iterator iter_map = mmapEventListeners.find(strListenName);
	if (iter_map != mmapEventListeners.end()) {
		std::pair<std::set<CEventListener*>::iterator, bool> pair_handler;
		pair_handler = iter_map->second.insert(pListener);
		if (!pair_handler.second) {
			LOG_ERR("CEventMgr::AddListener Add Listener Err");
			return 1;
		}
	} else {
		std::set<CEventListener*> setTmp;
		setTmp.insert(pListener);
		mmapEventListeners.insert(make_pair(strListenName, setTmp));
	}

	REF(pListener);
	return 0;
}

int CEventMgr::DelListener(CEventListener* pListener) {
	ASSERT_RET_VALUE(NULL != pListener, 1);
	bool bFlag = false;
	mcMapEventListenersMutex.Lock();
	for (std::map<std::string, std::set<CEventListener*>>::iterator iter_map = mmapEventListeners.begin(); iter_map != mmapEventListeners.end(); ++iter_map) {
		std::set<CEventListener*>::iterator iter_set = iter_map->second.find(pListener);
		if (iter_set != iter_map->second.end()) {
			iter_map->second.erase(iter_set);
			CEventListener* pTmpListener = pListener;
			UNREF(pTmpListener);
			bFlag = true;
		}
	}
	mcMapEventListenersMutex.UnLock();

	if (!bFlag) {
		LOG_ERR("Listener Not Exist:%p", pListener);
	}
	return 0;
}

int CEventMgr::DispatchEvent(std::string strListenName, void* pParam) {
	LOG_INFO("Enter CEventMgr::DispatchEvent:%s", strListenName.c_str());
	mcMapEventListenersMutex.Lock();
	for (std::map<std::string, std::set<CEventListener*>>::iterator iter_map = mmapEventListeners.begin(); iter_map != mmapEventListeners.end(); ++iter_map) {
		for (std::set<CEventListener*>::iterator iter_set = iter_map->second.begin(); iter_set != iter_map->second.end(); ++iter_set) {
			CEventListener* pListener = *iter_set;
			if (NULL != pListener) {
				pListener->OnEvent(pParam);
			}
		}
	}
	mcMapEventListenersMutex.UnLock();

	return 0;
}