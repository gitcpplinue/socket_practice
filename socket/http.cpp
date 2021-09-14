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

#include "http.h" 
#include "../tool/Log.h"
#include "../tool/Timer.h"

/*
#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
void* accept_request(void*);
void bad_request(int);
void cat(int, FILE *);
void cannot_execute(int);
void error_die(const char *);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void not_found(int);
void serve_file(int, const char *);
void unimplemented(int);
*/


void* accept_request(void* arg)
{
 int client = (long)arg; // 连接套接字
 char buf[1024];	// 缓冲区
 int numchars;		 
 char method[255];	// http方法
 char url[255];		// http url
 char path[512];	// 请求的文件路径
 size_t i, j;		 
 struct stat st;	// 文件状态 
 int cgi = 0;    	// http是否请求调用cgi程序
 char *query_string = NULL; // 用于带有请求体的GET方法

 // 读取第一行请求行
 numchars = get_line(client, buf, sizeof(buf));
 i = 0; j = 0;
 // 获取http方法
 while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
 {
  method[i] = buf[j];
  i++; j++;
 }
 method[i] = '\0';

 // 未定义方法
 if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
 {
  unimplemented(client);
  return NULL;
 }

 if (strcasecmp(method, "POST") == 0)
  cgi = 1;

 // 略过空格，获取url
 i = 0;
 while (ISspace(buf[j]) && (j < sizeof(buf)))
  j++;
 while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
 {
  url[i] = buf[j];
  i++; j++;
 }
 url[i] = '\0';

 // 将GET方法中'?'之后请求体与url分离，用query_string记录请求体
 if (strcasecmp(method, "GET") == 0)
 {
  query_string = url;
  while ((*query_string != '?') && (*query_string != '\0'))
   query_string++;
  if (*query_string == '?')
  {
   cgi = 1;
   *query_string = '\0';
   query_string++;
  }
 }

 // 获得文件在服务器上的路径
 sprintf(path, "./htdocs%s", url);
 // 默认文件路径
 if (path[strlen(path) - 1] == '/')
  strcat(path, "index.html");
 // 如果文件不存在，取出套接字中的首部行数据，发送http响应报文
 if (stat(path, &st) == -1) 
 {
  while ((numchars > 0) && strcmp("\n", buf)) 
   numchars = get_line(client, buf, sizeof(buf));
  not_found(client);
 }
 else
 {
  // 如果路径path是个目录，补上默认文件名
  if ((st.st_mode & S_IFMT) == S_IFDIR)
   strcat(path, "/index.html");
  // 该文件的所有者、所在组、其他人都拥有执行权限
  if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
   cgi = 1;

  // 根据cgi的标记决定发送html文件或是执行cgi程序
  if (!cgi)
   serve_file(client, path);
  else
   execute_cgi(client, path, method, query_string);
 }

 close(client);
}





// 普通的http响应报文
void bad_request(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n"
  "Content-type: text/html\r\n"
  "\r\n"
  "<P>Your browser sent a bad request, "
  "such as a POST without a Content-Length.\r\n");

 send(client, buf, sizeof(buf), 0);
}





// 获取文件的所有内容，功能与cat命令相同
void cat(int client, FILE *resource)
{
 printf("---------- cat ---------- \n");
 char buf[1024];

 fgets(buf, sizeof(buf), resource);
 while (!feof(resource))
 {
  printf("%s", buf);
  send(client, buf, strlen(buf), 0);
  fgets(buf, sizeof(buf), resource);
 }
 printf("\n");
}





// 普通的http响应报文
void cannot_execute(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n"
  "Content-type: text/html\r\n"
  "\r\n"
  "<P>Error prohibited CGI execution.\r\n");

 send(client, buf, strlen(buf), 0);
}





// 发送错误信息
void error_die(const char *sc)
{
 perror(sc);
 exit(1);
}





// 执行cgi程序
void execute_cgi(int client, const char *path,
                 const char *method, const char *query_string)
{
 char buf[1024] = { 0 };// 缓冲区
 int cgi_output[2];	// 管道，用于父子进程间的通信
 int cgi_input[2];	// 同上
 pid_t pid;		// 子进程id
 int status;		// 子进程的退出状态
 int i;		 
 char c;
 int numchars = 1;
 int content_length = -1;

// buf[0] = 'A'; buf[1] = '\0';

 // GET方法，从套接字取出首部行信息，不进行任何处理
 if (strcasecmp(method, "GET") == 0)
  while ((numchars > 0) && strcmp("\n", buf))  
   numchars = get_line(client, buf, sizeof(buf));
 else    // POST方法，从套接字取出首部行信息，
 {	 // 仅处理"Content-Length"字段
  numchars = get_line(client, buf, sizeof(buf));
  while ((numchars > 0) && strcmp("\n", buf))
  {
   buf[15] = '\0';
   // 获取"Content-Length"字段数据
   if (strcasecmp(buf, "Content-Length:") == 0)
    content_length = atoi(&(buf[16]));
   numchars = get_line(client, buf, sizeof(buf));
  }
  // 使用了POST请求却没有提供实体
  if (content_length == -1) 
  {
   bad_request(client);
   return;
  }
 }

 printf("---------- execute_cgi ---------- \n");
 // 回应报文的状态行
 sprintf(buf, "HTTP/1.0 200 OK\r\n");
 printf("%s", buf);
 send(client, buf, strlen(buf), 0);

 // 生成2个管道用于双向通信，创建子进程
 // cgi_output：用于子进程向父进程发送数据
 // cgi_input：用于父进程向子进程发送数据
 if (pipe(cgi_output) < 0) 
 {
  cannot_execute(client);
  return;
 }
 if (pipe(cgi_input) < 0) 
 {
  cannot_execute(client);
  return;
 }
 if ( (pid = fork()) < 0 ) 
 {
  cannot_execute(client);
  return;
 }

 // 子进程执行部分
 if (pid == 0)  
 {
  char meth_env[255];
  char query_env[255];
  char length_env[255];

  // 子进程的 stdout 重定向到 cgi_output 的输入端
  // 子进程的 stdin  重定向到 cgi_input  的读取端
  dup2(cgi_output[1], 1);
  dup2(cgi_input[0], 0);

  close(cgi_output[0]);
  close(cgi_input[1]);
  
  // 设置用于cgi程序的环境变量
  sprintf(meth_env, "REQUEST_METHOD=%s", method);
  putenv(meth_env);
  // GET
  if (strcasecmp(method, "GET") == 0) 
  {
   sprintf(query_env, "QUERY_STRING=%s", query_string);
   putenv(query_env);
  }
  // POST
  else 
  {    
   sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
   putenv(length_env);
  }

  // 执行cgi程序
  // cgi程序替换当前的子进程执行，保留了子进程的运行环境
  execl(path, path, NULL);
  exit(0);
 } 
 // 父进程执行部分
 else 
 {    
  close(cgi_output[1]);
  close(cgi_input[0]);

  // 之前的循环已将首部行情况，套接字里只剩下实体部分（如果存在的话）
  if (strcasecmp(method, "POST") == 0)
   // 从套接字获取实体部分数据，以c作为中介通过管道传给cgi进程
   for (i = 0; i < content_length; i++) 
   {
    recv(client, &c, 1, 0);
    printf("%c", c);
    write(cgi_input[1], &c, 1);
   }

  // 从cgi进程获取程序的执行结果，传入套接字
  while (read(cgi_output[0], &c, 1) > 0)
  {
   printf("%c", c);
   send(client, &c, 1, 0);
  }

  printf("\n\n");
  close(cgi_output[0]);
  close(cgi_input[1]);

  // 等待子进程执行完毕
  waitpid(pid, &status, 0);
 }
}





// 从套接字中获取一行数据
int get_line(int sock, char *buf, int size)
{
 int i = 0;
 char c = '\0';
 int n;

 // 读取size-1个字节或遇到'\n'时停止
 while ((i < size - 1) && (c != '\n'))
 {
  // 取1字节存入&c
  n = recv(sock, &c, 1, 0);
  if (n > 0)
  {
   // 遇到'\r'时，先查看下一个字符是否是'\n'，不取走数据
   if (c == '\r')
   {
    n = recv(sock, &c, 1, MSG_PEEK);
    // 如果遇到了"\r\n"，舍弃'\r'，只读取'\n'
    if ((n > 0) && (c == '\n'))
     recv(sock, &c, 1, 0);
    // 否则用'\n'替换'\r'
    else
     c = '\n';
   }

   // 间数据存入缓冲区
   buf[i] = c;
   i++;
  }
  // 套接字中没有数据可取，用'\n'结尾
  else
   c = '\n';
 }

 // 最后buf[size-1]位置由'\0'填补
 buf[i] = '\0';

 // 获取http报文的同时将其输出到屏幕上
 printf("%s", buf);
 
 return i;
}





// http状态行 
void headers(int client, const char *filename)
{
 printf("---------- headers ---------- \n");
 char buf[1024];
 (void)filename;

 strcpy(buf, "HTTP/1.0 200 OK\r\n"
  SERVER_STRING
  "Content-Type: text/html\r\n"
  "\r\n");

 printf("%s", buf);
 send(client, buf, strlen(buf), 0);
}





// 普通的http响应报文
void not_found(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n"
  SERVER_STRING
  "Content-Type: text/html\r\n"
  "\r\n"
  "<HTML><TITLE>Not Found</TITLE>\r\n"
  "<BODY><P>The server could not fulfill\r\n"
  "your request because the resource specified\r\n"
  "is unavailable or nonexistent.\r\n"
  "</BODY></HTML>\r\n");

 send(client, buf, strlen(buf), 0);
}




// 返回http响应报文和html文件内容
void serve_file(int client, const char *filename)
{
 FILE *resource = NULL;
 int numchars = 1;
 char buf[1024];

// buf[0] = 'A'; buf[1] = '\0';
 // 清空套接字内剩余的首部行
 while ((numchars > 0) && strcmp("\n", buf)) 
  numchars = get_line(client, buf, sizeof(buf));

 resource = fopen(filename, "r");
 if (resource == NULL)
  not_found(client);
 else
 {
  headers(client, filename);
  cat(client, resource);
 }
 fclose(resource);
}




// 普通的http响应报文
void unimplemented(int client)
{
 char buf[1024];

 sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n"
  SERVER_STRING
  "Content-Type: text/html\r\n"
  "\r\n"
  "<HTML><HEAD><TITLE>Method Not Implemented\r\n"
  "</TITLE></HEAD>\r\n"
  "<BODY><P>HTTP request method not supported.\r\n"
  "</BODY></HTML>\r\n");

 send(client, buf, strlen(buf), 0);
}


