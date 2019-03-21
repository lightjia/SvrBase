#ifndef __CUVUTIL__H_
#define __CUVUTIL__H_
#include "singleton.h"
#include "uv.h"
#include "util.h"
class CUvUtil : public CSingleton<CUvUtil>{
    SINGLE_CLASS_INITIAL(CUvUtil);
public:
    ~CUvUtil();

public:
    unsigned int UvVersion();
    const char* UvVersionStr();

    uv_pid_t GetPid();
    uv_pid_t GetPpid();
    std::string GetProcessTitle();
    int SetProcessTitle(const char* title);
    size_t GetProcessLimitMemory();
    uint64_t GetFreeMemory();
    uint64_t GetTotalMemory();
    int GetrUsage(uv_rusage_t* rusage);
    uint64_t GetHrTime();
    double GetProcessUptime();
    int ChDir(const char* dir);

    uv_os_fd_t GetOsFHandle(int iFd);

    int GetPasswd(uv_passwd_t* pwd);
    void FreePasswd(uv_passwd_t* pwd);

    int GetCpuInfo(uv_cpu_info_t** cpu_infos, int* count);
    void FreeCpuInfo(uv_cpu_info_t* cpu_infos, int count);

    int GetInterfaceAddr(uv_interface_address_t** addresses, int* count);
    void FreeInterfaceAddr(uv_interface_address_t* addresses, int count);

    std::string GetOsHomeDir();
    std::string GetTmpDir();
    std::string GetHostName();
    std::string GetExePath();
    std::string GetCwd();

    std::string GetEnv(const char* name);
    int SetEnv(const char* name, const char* value);
    int UnSetEnv(const char* name);

    int Ip4Addr(const char* ip, int port, struct sockaddr_in* addr);
    int Ip6Addr(const char* ip, int port, struct sockaddr_in6* addr);
    std::string Ip4Name(const struct sockaddr_in* src);
    std::string Ip6Name(const struct sockaddr_in6* src);
    int InetNtop(int af, const void* src, char* dst, size_t size);
    int InetPton(int af, const char* src, void* dst);
};
#define sUvUtil CUvUtil::Instance()
#endif