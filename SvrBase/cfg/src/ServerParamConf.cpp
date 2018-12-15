#include "ServerParamConf.h"

#define CONF_FILE_NAME "server_param.json"

CServerParamConf::CServerParamConf()
{
}

CServerParamConf::~CServerParamConf()
{
}

int CServerParamConf::Init()
{
	if (!m_bLoad && 0 == LoadConfig(CONF_FILE_NAME, CONF_FILE_TYPE_JSON))
	{
		m_bLoad = true;
	}

	return 0;
}

int CServerParamConf::UnInit()
{
	for (std::map<int, tagSvrParamConf*>::iterator iter = m_mapConf.begin(); iter != m_mapConf.end();)
	{
		tagSvrParamConf* pConf = (tagSvrParamConf*)iter->second;
		if (SVR_TYPE_STR == pConf->iType)
		{
			DOFREE(pConf->value.strValue);
		}

		DOFREE(pConf);
		m_mapConf.erase(iter++);
	}
	return 0;
}

int CServerParamConf::GetIntConf(int id, int iDefault)
{
	int iRet = iDefault;
	std::map<int, tagSvrParamConf*>::iterator iter = m_mapConf.find(id);
	if (iter != m_mapConf.end())
	{
		iRet = iter->second->value.iValue;
	}

	return iRet;
}

double CServerParamConf::GetFloatConf(int id, double fDefault)
{
	double fRet = fDefault;
	std::map<int, tagSvrParamConf*>::iterator iter = m_mapConf.find(id);
	if (iter != m_mapConf.end())
	{
		fRet = iter->second->value.fValue;
	}

	return fRet;
}

const char* CServerParamConf::GetStrConf(int id, const char* strDefault)
{
	const char* pRet = strDefault;
	std::map<int, tagSvrParamConf*>::iterator iter = m_mapConf.find(id);
	if (iter != m_mapConf.end())
	{
		pRet = iter->second->value.strValue;
	}

	return pRet;
}

int CServerParamConf::ParseJson(rapidjson::Document& doc)
{
	if (doc.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
		{
			rapidjson::Value &v = doc[i];

			if (v.HasMember("valuetype") && v.HasMember("ID") && v.HasMember("value"))
			{
				fprintf(stderr, "CServerParamConf::ParseJson malloc\n");
				tagSvrParamConf* pConf = (tagSvrParamConf*)do_malloc(sizeof(tagSvrParamConf));
				pConf->iId = v["ID"].GetInt();
				const char* pValueType = v["valuetype"].GetString();
				if (str_cmp(pValueType, CONF_INT_TYPE, true))
				{
					pConf->value.iValue = v["value"].GetInt();
				}
				else if (str_cmp(pValueType, CONF_DOUBLE_TYPE, true))
				{
					pConf->value.fValue = v["value"].GetDouble();
				}
				else if (str_cmp(pValueType, CONF_STRING_TYPE, true))
				{
					//safe_snprintf(pConf->value.strValue, CONF_MAX_STR_LEN - 1, _TRUNCATE, "%s", v["value"].GetString());
					pConf->iType = SVR_TYPE_STR;
					pConf->value.strValue = _strdup(v["value"].GetString());
				}
				else
				{
					fprintf(stderr, "Error value type:%s", pValueType);
					DOFREE(pConf);
					continue;
				}

				std::pair<std::map<int, tagSvrParamConf*>::iterator, bool> pair_conf;
				pair_conf = m_mapConf.insert(std::pair<int, tagSvrParamConf*>(pConf->iId, pConf));
				if (!pair_conf.second)
				{
					fprintf(stderr, "insert error id:%d", pConf->iId);
					DOFREE(pConf);
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "%s parse error", CONF_FILE_NAME);
	}

	return 0;
}