#ifndef __CServerConfig__H_
#define __CServerConfig__H_
#include "Config.h"
#include "singleton.h"
#include <map>

enum svr_param_conf_id
{
	SVR_PARAM_CONF_ID_LOG_TYPE = 1,	//日志类型
	SVR_PARAM_CONF_ID_LOG_LEVEL,	//日志级别
	SVR_PARAM_CONF_ID_LOG_PATH,	//日志目录
	SVR_PARAM_CONF_ID_SVR_PORT,	//服务器端口号
    SVR_PARAM_CONF_ID_SVR_IP,	//服务器IP
    SVR_PARAM_CONF_ID_TIMER_INTERVAL,   //定时器间隔
    SVR_PARAM_CONF_ID_RTSP_MODE,    //RTSP取流方式
    SVR_PARAM_CONF_ID_TCP_ALIVE_SEC,    //TCP保活时间单位是秒
    SVR_PARAM_CONF_ID_PERSON_IMG_DIR,   //检测图像存储目录
    SVR_PARAM_CONF_ID_PERSON_IMG_BAK_FLAG,  //是否缓存每一帧检测的图片 0=>不缓存 非0=>缓存
    SVR_PARAM_CONF_ID_PERSON_IMG_BAK_DIR,   //检测图像存储临时目录
};

#pragma pack(1)	//1字节对齐
#define SVR_TYPE_STR	1
struct tagSvrParamConf
{
	int iId;	//config ID
	unsigned char iType;	//SVR_TYPE_STR为str
	union Value
	{
		int iValue;
		double fValue;
		char* strValue;
	}value;
};
#pragma pack()	//取消一字节对齐

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