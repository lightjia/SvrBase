#include "Config.h"

CConfig::CConfig(){
	m_bLoad = false;
}

CConfig::~CConfig(){
}

int CConfig::LoadJsonConf(const char* pData){
	if (NULL == pData){
		return 1;
	}

	rapidjson::Document doc;
	doc.Parse<rapidjson::kParseDefaultFlags>(pData);
	if (!doc.HasParseError()) {
		ParseJson(doc);
	} else {
		fprintf(stderr, "Parse Json File Error:%s\n", pData);
	}

	return 0;
}

int CConfig::LoadConfig(const char* pFileName, conf_file_type iType){
	const long lFileLen = get_file_len(pFileName, "r");
	if (lFileLen > 0){
		fprintf(stderr, "CConfig::LoadConfig Load:%s\n", pFileName);
		char* pFileData = (char*)MemMalloc(lFileLen + 1);
		size_t iRead = file_read(pFileName, "rb", (unsigned char*)pFileData, lFileLen, 0);
		if (iRead == lFileLen) {
			pFileData[iRead] = '\0';
			if (iType == CONF_FILE_TYPE_JSON){
				LoadJsonConf(pFileData);
			} else {
				fprintf(stderr, "error conf type:%d\n", iType);
			}
		} else {
			fprintf(stderr, "File:%s filelen:%ld readlen:%ld\n", pFileName, lFileLen, (unsigned long)iRead);
		}

		MemFree(pFileData);
	} else {
		fprintf(stderr, "file:%s is empty\n", pFileName);
		return 1;
	}

	return 0;
}
