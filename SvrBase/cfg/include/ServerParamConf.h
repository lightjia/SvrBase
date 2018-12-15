#ifndef __CServerConfig__H_
#define __CServerConfig__H_
#include "Config.h"
#include "singleton.h"
#include <map>

enum svr_param_conf_id
{
	SVR_PARAM_CONF_ID_LOG_TYPE = 1,	//��־����
	SVR_PARAM_CONF_ID_LOG_LEVEL,	//��־����
	SVR_PARAM_CONF_ID_LOG_PATH,	//��־Ŀ¼
	SVR_PARAM_CONF_ID_SVR_PORT,	//�������˿ں�
    SVR_PARAM_CONF_ID_SVR_IP,	//������IP
    SVR_PARAM_CONF_ID_TIMER_INTERVAL,   //��ʱ�����
    SVR_PARAM_CONF_ID_RTSP_MODE,    //RTSPȡ����ʽ
    SVR_PARAM_CONF_ID_TCP_ALIVE_SEC,    //TCP����ʱ�䵥λ����
    SVR_PARAM_CONF_ID_PERSON_IMG_DIR,   //���ͼ��洢Ŀ¼
    SVR_PARAM_CONF_ID_PERSON_IMG_BAK_FLAG,  //�Ƿ񻺴�ÿһ֡����ͼƬ 0=>������ ��0=>����
    SVR_PARAM_CONF_ID_PERSON_IMG_BAK_DIR,   //���ͼ��洢��ʱĿ¼
};

#pragma pack(1)	//1�ֽڶ���
#define SVR_TYPE_STR	1
struct tagSvrParamConf
{
	int iId;	//config ID
	unsigned char iType;	//SVR_TYPE_STRΪstr
	union Value
	{
		int iValue;
		double fValue;
		char* strValue;
	}value;
};
#pragma pack()	//ȡ��һ�ֽڶ���

class CServerParamConf : public CConfig, public CSingleton<CServerParamConf>
{
	SINGLE_CLASS_INITIAL(CServerParamConf);

public:
	~CServerParamConf();

public:
	int GetIntConf(int id, int iDefault);
	double GetFloatConf(int id, double fDefault);
	const char* GetStrConf(int id, const char* strDefault);

public:
	int Init();
	int UnInit();

protected:
	int ParseJson(rapidjson::Document& doc);

private:
	std::map<int, tagSvrParamConf*> m_mapConf;
};

#define sSvrParamConf CServerParamConf::Instance()
#endif