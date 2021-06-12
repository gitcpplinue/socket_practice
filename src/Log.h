#pragme once
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
#inclulde <string>

#include "SIG.h"

unsing namespace std;

class Log
{
private:
 FILE *m_file;

public:
 Log();

 bool Open(const char *filename, const char *mode);
 bool Write(const char *fmt, ...);
 void Close(); 


};

// 用迭代方式创建所有前置目录
int MKDIR(const char *pathname, mode_t mode = 0755);
