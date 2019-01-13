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
		fprintf(stderr, "Parse Json File Error");
	}

	return 0;
}

int CConfig::LoadConfig(const char* pFileName, conf_file_type iType){
	const long lFileLen = get_file_len(pFileName, "r");
	if (lFileLen > 0){
		fprintf(stderr, "CConfig::LoadConfig malloc\n");
		char* pFileData = (char*)do_malloc(lFileLen + 1);
		size_t iRead = file_read(pFileName, "rb", (unsigned char*)pFileData, lFileLen, 0);
		if (iRead == lFileLen) {
			pFileData[iRead] = '\0';
			if (iType == CONF_FILE_TYPE_JSON){
				LoadJsonConf(pFileData);
			} else {
				fprintf(stderr, "error conf type:%d", iType);
			}
		} else {
			fprintf(stderr, "File:%s filelen:%ld readlen:%lld", pFileName, lFileLen, iRead);
		}

		DOFREE(pFileData);
	} else {
		fprintf(stderr, "file:%s is empty", pFileName);
	}

	return 0;
}