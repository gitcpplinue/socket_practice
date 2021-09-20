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

/*
 * 对TinyHttp项目源码进行了封装，只对部分细节进行了修改
 * 每有1个新客户接入服务端，创建一个新线程，新线程创建1个http实例
 */
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

 Log *m_log;		// 日志类指针
 Timer *m_timer;	// 计时器类指针，现在还用不到

// int m_pipes[4];

public:
 Http();
 ~Http();

 // 从套接字中读取一行数据
 int get_line(int socket, char * buf, int size);

 // 解析、处理http报文
 void accept_request(int socket);

 // 客户端使用POST方法，执行cgi程序，返回程序运行结果
 void execute_cgi(int client, const char *path, const char *method, const char *query_string);

 // 客户端使用GET方法，获取html文件，传给套接字
 void serve_file(int client, const char * filename);

 // 向套接字传入文件内容
 void cat(int client, const char* filename);

// int startup(u_short *);

 // 以下5个函数都只是简单地发出http回应报文
 void headers(int client, const char *filename);
 void bad_request(int client);
 void cannot_execute(int client);
 void not_found(int client);
 void unimplemented(int client);

 // 打印错误信息并结束进程
 void error_die(const char *sc);

 // 线程清理函数，关闭可能打开的文件
 void shutdown(void*);
};
