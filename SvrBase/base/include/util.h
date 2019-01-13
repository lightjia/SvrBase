#ifndef __UTIL__H_
#define __UTIL__H_
#include <algorithm>
#include "common.h"
#include "LocalDefine.h"

//mem
#define DOFREE(X) do{if(NULL != X) {free(X); X = NULL;}}while(0)
#define DODELETE(X) do{if(NULL != X) {delete X; X = NULL;}}while(0)
#define BZERO(X) memset(&X, 0, sizeof(X))
void* do_malloc(size_t iLen);

//time
struct systemtime_t get_now_time();
uint64_t get_time_ms();	//获取时间毫秒级
void sleep_ms(DWORD dwMillions);

//thread
DWORD get_thread_id_self();
long atomic_change(volatile long* value, int off);

//strings
bool str_cmp(const char* pStr1, const char* pStr2, bool bIgnoreCase);
bool str_start_with(const std::string& str, const std::string& prefix);
bool str_end_with(const std::string& str, const std::string& suffix);
bool str_cmp_nocase(std::string str1, std::string str2);
std::string& str_ltrim(std::string& str);
std::string& str_rtrim(std::string& str);
std::string& str_trim(std::string& str);
std::string str_2_lower(std::string& str);
std::string str_2_upper(std::string& str);
wchar_t* char_2_wchar(const char* pStr);
char* wchar_2_char(const wchar_t* pStr);
int split_str(std::string strSource, std::string strSep, std::vector<std::string>& vecRet);

//files
FILE* safe_open_file(const char* pFileName, const char* pMode);
len_str get_bmp(const void* pBmp, int width, int height, int bitCount);
const long get_file_len(const char* pFileName, const char* pMode);
size_t file_write(const char* pFileName, const char* pMode, const unsigned char* pData, const size_t iLen);
size_t file_read(const char* pFileName, const char* pMode, unsigned char* pData, const size_t iLen, const long lOffset);
char* file_read(const char* pFileName, const char* pMode, size_t& lLen);
std::string get_app_path();
bool file_exits(const char*  strFileName);
bool directory_exists(const char* strDirectory);
int make_dir(const char* path);	//创建目录
int make_dirs(const char* path);	//创建多层目录
int rm_file(const char* pFileName);
int rm_dir(const char* pDirName);

//network
int init_platform();
int uninit_platform();

//error
void perror_desc(const char* pStr);
#endif