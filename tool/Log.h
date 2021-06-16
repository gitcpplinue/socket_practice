#pragma once
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <string>
#include <stdarg.h>
#include <pthread.h>

#include "SIG.h"
#include "Timer.h"

using namespace std;

class Log
{
private:
 FILE *m_file;
 pthread_mutex_t m_mutex;

public:
 Log();
 ~Log();
 
 // 创建或打开日志文件
 bool Open(const char *filename, const char *mode);
 // 向日志文件写数据
 bool Write(const char *fmt, ...);
 // 关闭日志文件
 void Close(); 
 void FreeLock(){ pthread_mutex_unlock(&m_mutex); }

/* 
* log.Open("log.txt", "w");
* log.Write("a test for first log file %d %d\n", 1, 2);
* log.Close();
*/


};

// 用迭代方式创建所有前置目录
int MKDIR(const char *pathname, mode_t mode = 0755);
