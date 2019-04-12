#ifndef __CJSONREADHANDLER__H_
#define __CJSONREADHANDLER__H_
#include "rapidjson/reader.h"
#include <iostream>
#include <sstream>
#include "Log.h"

//{ \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] }
class CJsonReadHandler
{
    DISALLOW_COPY_AND_ASSIGN(CJsonReadHandler);

public:
    CJsonReadHandler();
    virtual ~CJsonReadHandler();

public:
    int StartParseJson(const char* pData);

protected:
    virtual bool GetNull() { return true; }
    virtual bool GetBool(bool b) { return true; }
    virtual bool GetInt(int i) { return true; }
    virtual bool GetUint(unsigned u) { return true; }
    virtual bool GetInt64(int64_t i) { return true; }
    virtual bool GetUint64(uint64_t u) { return true; }
    virtual bool GetDouble(double d) { return true; }
    virtual bool GetRawNumber(const char* str, rapidjson::SizeType length) { return true; }
    virtual bool GetString(const char* str, rapidjson::SizeType length) { return true; }
    virtual bool GetStartObject() { return true; }
    virtual bool GetKey(const char* str, rapidjson::SizeType length) { return true; }
    virtual bool GetEndObject(rapidjson::SizeType memberCount) { return true; }
    virtual bool GetStartArray() { return true; }
    virtual bool GetEndArray(rapidjson::SizeType elementCount) { return true; }

public:
    bool Null() { return GetNull(); }
    bool Bool(bool b) { return GetBool(b); }
    bool Int(int i) { return GetInt(i); }
    bool Uint(unsigned u) { return GetUint(u); }
    bool Int64(int64_t i) { return GetInt64(i); }
    bool Uint64(uint64_t u) { return GetUint64(u); }
    bool Double(double d) { return GetDouble(d); }
    bool RawNumber(const char* str, rapidjson::SizeType length, bool) { return GetRawNumber(str, length); }
    bool String(const char* str, rapidjson::SizeType length, bool) { return GetString(str, length); }
    bool StartObject() { return GetStartObject(); }
    bool Key(const char* str, rapidjson::SizeType length, bool) {  return GetKey(str, length); }
    bool EndObject(rapidjson::SizeType memberCount) {  return GetEndObject(memberCount); }
    bool StartArray() { return GetStartArray(); }
    bool EndArray(rapidjson::SizeType elementCount) {  return GetEndArray(elementCount); }
};

#endif