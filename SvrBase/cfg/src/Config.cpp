#include "Config.h"

#define DEFAULT_CONF_DIR	"conf/"

CConfig::CConfig()
{
	m_bLoad = false;
}

CConfig::~CConfig()
{
}

int CConfig::LoadJsonConf(const char* pData)
{
	if (nullptr == pData)
	{
		return 1;
	}

	rapidjson::Document doc;
	doc.Parse<rapidjson::kParseDefaultFlags>(pData);
	if (!doc.HasParseError())
	{
		ParseJson(doc);
	}
	else
	{
		fprintf(stderr, "Parse Json File Error");
	}

	return 0;
}

int CConfig::LoadConfig(const char* pFileName, conf_file_type iType)
{
	std::string strConfFile = get_app_path() + DEFAULT_CONF_DIR + pFileName;
	const long lFileLen = get_file_len(strConfFile.c_str(), "r");
	if (lFileLen > 0)
	{
		fprintf(stderr, "CConfig::LoadConfig malloc\n");
		char* pFileData = (char*)do_malloc(lFileLen);
		file_read(strConfFile.c_str(), "r", (unsigned char*)pFileData, lFileLen, 0);
		if (iType == CONF_FILE_TYPE_JSON)
		{
			LoadJsonConf(pFileData);
		}
		else
		{
			fprintf(stderr, "error conf type:%d", iType);
		}

		DOFREE(pFileData);
	}
	else
	{
		fprintf(stderr, "file:%s is empty", pFileName);
	}

	return 0;
}