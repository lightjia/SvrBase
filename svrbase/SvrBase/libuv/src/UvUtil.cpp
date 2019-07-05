#include "UvUtil.h"
#include "MemMgr.h"
#include "MemBuffer.h"

#define UV_UTIL_BUF_LEN 1024
typedef int(*uv_oper_fun)(char* buffer, size_t* size);

std::string UvFunc(uv_oper_fun pFunc) {
    size_t iRealLen = UV_UTIL_BUF_LEN;
    int iRet = 0;
	CMemBuffer cBuffer;
    do {
		cBuffer.AllocBuffer(iRealLen * sizeof(char));
        int iRet = 0;
        if (iRet = pFunc((char*)cBuffer.GetBuffer(), &iRealLen)) {
            iRealLen = 0;
            if (UV_ENOBUFS == iRet) {
				iRealLen *= 2;
            } else {
                break;
            }
        }
    } while (iRet);

    std::string strRet((char*)cBuffer.GetBuffer(), iRealLen);
    return strRet;
}

CUvUtil::CUvUtil(){
}

CUvUtil::~CUvUtil(){
}

unsigned int CUvUtil::UvVersion() {
    return uv_version();
}

const char* CUvUtil::UvVersionStr() {
    return uv_version_string();
}

std::string CUvUtil::GetOsHomeDir() {
    return UvFunc(uv_os_homedir);
}

std::string CUvUtil::GetTmpDir() {
    return UvFunc(uv_os_tmpdir);
}

std::string CUvUtil::GetHostName() {
    return UvFunc(uv_os_gethostname);
}

std::string CUvUtil::GetExePath() {
    return UvFunc(uv_exepath);
}

std::string CUvUtil::GetCwd() {
    return UvFunc(uv_cwd);
}

uv_pid_t CUvUtil::GetPid() {
    return uv_os_getpid();
}

uv_pid_t CUvUtil::GetPpid() {
    return uv_os_getppid();
}

std::string CUvUtil::GetProcessTitle() {
    size_t iRealLen = UV_UTIL_BUF_LEN;
    int iRet = 0;
	CMemBuffer cBuffer;
    do {
        cBuffer.AllocBuffer(iRealLen * sizeof(char));
        int iRet = 0;
        if (iRet = uv_get_process_title((char*)cBuffer.GetBuffer(), iRealLen)) {
            iRealLen = 0;
            if (UV_ENOBUFS == iRet) {
				iRealLen *= 2;
            }
            else {
                break;
            }
        }
    } while (iRet);

    std::string strRet((char*)cBuffer.GetBuffer(), iRealLen);
    return strRet;
}

int CUvUtil::SetProcessTitle(const char* title) {
    return uv_set_process_title(title);
}

size_t CUvUtil::GetProcessLimitMemory() {
    size_t iRet = 0;
    uv_resident_set_memory(&iRet);
    return iRet;
}

uint64_t CUvUtil::GetFreeMemory() {
    return uv_get_free_memory();
}

uint64_t CUvUtil::GetTotalMemory() {
    return uv_get_total_memory();
}

int CUvUtil::GetrUsage(uv_rusage_t* rusage) {
    return uv_getrusage(rusage);
}

uint64_t CUvUtil::GetHrTime() {
    return uv_hrtime();
}

double CUvUtil::GetProcessUptime() {
    double dRet = 0;
    uv_uptime(&dRet);
    return dRet;
}

int CUvUtil::ChDir(const char* dir) {
    return uv_chdir(dir);
}

uv_os_fd_t CUvUtil::GetOsFHandle(int iFd) {
    return uv_get_osfhandle(iFd);
}

int CUvUtil::GetPasswd(uv_passwd_t* pwd) {
    return uv_os_get_passwd(pwd);
}

void CUvUtil::FreePasswd(uv_passwd_t* pwd) {
    uv_os_free_passwd(pwd);
}

int CUvUtil::GetCpuInfo(uv_cpu_info_t** cpu_infos, int* count) {
    return uv_cpu_info(cpu_infos, count);
}

void CUvUtil::FreeCpuInfo(uv_cpu_info_t* cpu_infos, int count) {
    uv_free_cpu_info(cpu_infos, count);
}

int CUvUtil::GetInterfaceAddr(uv_interface_address_t** addresses, int* count) {
    return uv_interface_addresses(addresses, count);
}

void CUvUtil::FreeInterfaceAddr(uv_interface_address_t* addresses, int count) {
    uv_free_interface_addresses(addresses, count);
}

std::string CUvUtil::GetEnv(const char* name) {
    size_t iRealLen = UV_UTIL_BUF_LEN;
    int iRet = 0;
	CMemBuffer cBuffer;
    do {
		cBuffer.AllocBuffer(iRealLen * sizeof(char));
        int iRet = 0;
        if (iRet = uv_os_getenv(name, (char*)cBuffer.GetBuffer(), &iRealLen)) {
            iRealLen = 0;
            if (UV_ENOBUFS == iRet) {
				iRealLen *= 2;
            } else {
                break;
            }
        }
    } while (iRet);

    std::string strRet((char*)cBuffer.GetBuffer(), iRealLen);
    return strRet;
}

int CUvUtil::SetEnv(const char* name, const char* value) {
    return uv_os_setenv(name, value);
}

int CUvUtil::UnSetEnv(const char* name) {
    return uv_os_unsetenv(name);
}

int CUvUtil::Ip4Addr(const char* ip, int port, struct sockaddr_in* addr) {
    return uv_ip4_addr(ip, port, addr);
}

int CUvUtil::Ip6Addr(const char* ip, int port, struct sockaddr_in6* addr) {
    return uv_ip6_addr(ip, port, addr);
}

#define UV_IP_DEFINE_LEN 380
std::string CUvUtil::Ip4Name(const struct sockaddr_in* src){
    char ip[UV_IP_DEFINE_LEN] = "\0";
    size_t iIpLen = UV_IP_DEFINE_LEN;
    uv_ip4_name(src, ip, iIpLen);

    return std::string(ip, iIpLen);
}

std::string CUvUtil::Ip6Name(const struct sockaddr_in6* src) {
    char ip[UV_IP_DEFINE_LEN] = "\0";
    size_t iIpLen = UV_IP_DEFINE_LEN;
    uv_ip6_name(src, ip, iIpLen);

    return std::string(ip, iIpLen);
}

int CUvUtil::InetNtop(int af, const void* src, char* dst, size_t size) {
    return uv_inet_ntop(af, src, dst, size);
}

int CUvUtil::InetPton(int af, const char* src, void* dst) {
    return uv_inet_pton(af, src, dst);
}