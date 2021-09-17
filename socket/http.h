#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <vector>

#include "../tool/Log.h"
#include "../tool/Timer.h"
#include "../tool/Redis.h"

#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"


class Http
{
private:
 int m_client; 		// 客户端套接字
 char m_buf[1024]; 	// 缓冲区
 char m_method[255]; 	// 方法
 char m_url[255]; 	// http的url
 char m_path[512];      // 文件路径
 int m_cgi;      	// 标记是否要执行cgi程序
 char *m_query_string;	// GET方法的请求报文
 FILE *m_htmlfp; 	// cat函数打开的文件的描述符

 Log *m_log;
 Timer *m_timer;

// int m_pipes[4];

public:
 Http();
 ~Http();

 void accept_request(int);
 void bad_request(int);
 void cat(int, const char*);
 void cannot_execute(int);
 void error_die(const char *);
 void execute_cgi(int, const char *, const char *, const char *);
 int get_line(int, char *, int);
 void headers(int, const char *);
 void not_found(int);
 void serve_file(int, const char *);
 int startup(u_short *);
 void unimplemented(int);

 void shutdown(void*);
};
