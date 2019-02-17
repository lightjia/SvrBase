#ifndef __COMMON__H_
#define __COMMON__H_
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <map>
#include <vector>

#if (defined WIN32) || (defined _WIN32) ||(defined WIN64) || (defined _WIN64)
#define PLATFORM_WINDOWS
#elif (defined __linux__)
#define PLATFORM_LINUX
#endif

#if (defined PLATFORM_WINDOWS)
#include <io.h>
#include <Ws2tcpip.h>
#include <winsock2.h>  
#include <windows.h>
#include <process.h>  
#include <sys/timeb.h>
#elif  (defined PLATFORM_LINUX)
#include<stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <sys/types.h>   
#include <arpa/inet.h> 
#include <errno.h>  
#include <dirent.h>
#endif

#endif