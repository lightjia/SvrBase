#include "JsonTool.h"

CJsonTool::CJsonTool()
{
}

CJsonTool::~CJsonTool()
{
}

int CJsonTool::DecodeJsonArray(const char* pJson, std::map<std::string, std::string>& mapJson)
{
	int iRet = 1;
	mapJson.clear();
	do 
	{
		if(nullptr == pJson) break;
		rapidjson::Document doc;
		doc.Parse<rapidjson::kParseDefaultFlags>(pJson);
		if (doc.HasParseError()) break;
		if (!doc.IsArray()) break;
		for (rapidjson::SizeType i = 0; i < doc.Size(); i++)
		{
			rapidjson::Value &v = doc[i];
			for (rapidjson::Value::ConstMemberIterator itr = v.MemberBegin(); itr != v.MemberEnd(); itr++)
			{
				rapidjson::Value jKey;
				rapidjson::Value jValue;
				rapidjson::Document::AllocatorType allocator;
				jKey.CopyFrom(itr->name, allocator);
				jValue.CopyFrom(itr->value, allocator);
				if (jKey.IsString() && jValue.IsString())
				{
					mapJson.insert(std::pair<std::string, std::string>(jKey.GetString(), jValue.GetString()));
				}
			}
		}

		iRet = 0;
	} while (0);
	
	return iRet;
}

int CJsonTool::EncodeJsonArray(std::string& strRet, std::map<std::string, std::string>& mapJson)
{
	rapidjson::Document jsonDoc;    //����һ��domԪ��Document
	jsonDoc.SetArray();
	rapidjson::Document::AllocatorType &allocator = jsonDoc.GetAllocator(); //��ȡ������
	//rapidjson::Value jsonArray(rapidjson::kArrayType);//����һ��Array���͵�Ԫ��
	for (std::map<std::string, std::string>::iterator iter = mapJson.begin(); iter != mapJson.end(); ++iter)
	{
		rapidjson::Value jsonMap(rapidjson::kObjectType);
		jsonMap.AddMember(rapidjson::StringRef(iter->first.c_str()), rapidjson::StringRef(iter->second.c_str()), allocator);
		//jsonArray.PushBack(jsonMap, allocator);
		jsonDoc.PushBack(jsonMap, allocator);
	}
	
	//�����ַ���
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	jsonDoc.Accept(writer);
	strRet = buffer.GetString();

	return 0;
}