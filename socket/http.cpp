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
#include <vector>
#include "http.h" 

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




Http::Http()
{
 m_client = 0;
 m_cgi = 0;
 m_query_string = NULL;
 m_htmlfp = NULL;
 m_log = new Log;
 m_timer = new Timer;
}

Http::~Http()
{
 delete m_log;
 delete m_timer;
}




 
/*
 * 从套接字中获取一行数据
 * 为了识别"\r\n"，以1字节为单位一个个读取
 */
int Http::get_line(int sock, char *m_buf, int size)
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

   // 将数据存入缓冲区
   m_buf[i] = c;
   i++;
  }
  // 套接字中没有数据可取，用'\n'结尾
  else
   c = '\n';
 }

 // 最后m_buf[size-1]位置由'\0'填补
 m_buf[i] = '\0';

 // 获取http报文的同时将其写入日志
 m_log->Write("%s", m_buf);
 
 return i;
}




/*
 * 解析http报文的第1行
 * 根据解析结果决定下一步调用的函数
 */
void Http::accept_request(int socket)
{
 int m_client = socket; // 连接套接字
 int numchars;		 
 size_t i, j;		 
 struct stat st;	// 文件状态 

 /* 根据当前时间生成日志文件路径并打开 */ 
 time_t ct;
 tm *tmp;
 char logfile[20] = { 0 };
 char logpath[50] = { 0 };
 time(&ct);
 tmp = localtime(&ct);

 strftime(logfile, 50, "%y%m%d", tmp);
 sprintf(logpath, "./log/web/%s", logfile);
 m_log->Open(logpath, "a");


 // 读取第一行请求行
 numchars = get_line(m_client, m_buf, sizeof(m_buf));
 i = 0; j = 0;
 // 获取http方法
 while (!ISspace(m_buf[j]) && (i < sizeof(m_method) - 1))
 {
  m_method[i] = m_buf[j];
  i++; j++;
 }
 m_method[i] = '\0';

 // 如果是GET和POST以外的方法
 if (strcasecmp(m_method, "GET") && strcasecmp(m_method, "POST"))
 {
  unimplemented(m_client);
  return;
 }

 if (strcasecmp(m_method, "POST") == 0)
  m_cgi = 1;

 // 略过空格，获取url
 i = 0;
 while (ISspace(m_buf[j]) && (j < sizeof(m_buf)))
  j++;
 while (!ISspace(m_buf[j]) && (i < sizeof(m_url) - 1) && (j < sizeof(m_buf)))
 {
  m_url[i] = m_buf[j];
  i++; j++;
 }
 m_url[i] = '\0';

 // 将GET方法中'?'之后请求体与url分离，用m_query_string记录请求体
 if (strcasecmp(m_method, "GET") == 0)
 {
  m_query_string = m_url;
  while ((*m_query_string != '?') && (*m_query_string != '\0'))
   m_query_string++;
  if (*m_query_string == '?')
  {
   m_cgi = 1;
   *m_query_string = '\0';
   m_query_string++;
  }
 }

 // 文件在服务器中的路径
 sprintf(m_path, "./htdocs%s", m_url);
 // 默认文件"index.html"
 if (m_path[strlen(m_path) - 1] == '/')
  strcat(m_path, "index.html");
 // 如果文件不存在，取出套接字中的剩余数据，发送http响应报文
 if (stat(m_path, &st) == -1) 
 {
  while ((numchars > 0) && strcmp("\n", m_buf)) 
   numchars = get_line(m_client, m_buf, sizeof(m_buf));
  not_found(m_client);
 }
 else
 {
  // 如果路径m_path是个目录，补上默认文件名
  if ((st.st_mode & S_IFMT) == S_IFDIR)
   strcat(m_path, "/index.html");
 // 如果该文件的所有者、所在组、其他人都拥有执行权限，将其视为cgi文件
  if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
   m_cgi = 1;

  // 根据m_cgi的标记决定发送html文件或是执行m_cgi程序
  if (!m_cgi)
   serve_file(m_client, m_path);
  else
   execute_cgi(m_client, m_path, m_method, m_query_string);
 }

 close(m_client);
}





/*
 * 根据客户端的请求设置cgi程序的环境变量。
 * 生成子进程运行cgi程序，父进程使用管道与子进程交互。
 * 父进程获得cgi程序（子进程）的输出，将结果传入套接字
 * 优先使用redis缓存中的数据
 */
void Http::execute_cgi(int m_client, const char *m_path,
                 const char *m_method, const char *m_query_string)
{
 char m_buf[1024] = { 0 };// 缓冲区
 int cgi_output[2];	// 管道，用于父子进程间的通信
 int cgi_input[2];	// 同上
 pid_t pid;		// 子进程id
 int status;		// 子进程的退出状态
 int i;		 
 char c;
 int numchars = 1;
 int content_length = -1;
 string entity_body;
 // 执行redis数据库查询需要的字符串
 string hkey;
 string hfiled;
 string hvalue;


 hkey = m_method; // hash数据结构key关键字

 // GET方法，从套接字取出首部行信息，不进行任何处理
 if (strcasecmp(m_method, "GET") == 0)
 {
  while ((numchars > 0) && strcmp("\n", m_buf))  
   numchars = get_line(m_client, m_buf, sizeof(m_buf));

  hfiled = m_query_string; // hash数据结构filed字段
 }
 else    // POST方法，从套接字取出首部行信息，
 {	 // 仅处理"Content-Length"字段
  numchars = get_line(m_client, m_buf, sizeof(m_buf));

  // 获取"Content-Length"字段数据
  while ((numchars > 0) && strcmp("\n", m_buf))
  {
   m_buf[15] = '\0';
   if (strcasecmp(m_buf, "Content-Length:") == 0)
    content_length = atoi(&(m_buf[16]));
   numchars = get_line(m_client, m_buf, sizeof(m_buf));
  }

  // 如果使用了POST请求却没有提供实体
  if (content_length == -1) 
  {
   bad_request(m_client);
   return;
  }
  // 从套接字中获取剩下的POST方法实体部分
  else
  {
   char c;
   entity_body.resize(content_length, '\0');
   for (i = 0; i < content_length; ++i)
   {
    recv(m_client, &c, 1, 0);
    entity_body[i] = c;    
   }
   m_log->Write("%s\n\n", entity_body.c_str());

   hfiled = entity_body; // hash数据结构filed字段
  }
 }

 m_log->Write("---------- execute_cgi ---------- \n");
 // http回应报文的状态行
 sprintf(m_buf, "HTTP/1.0 200 OK\r\n");
 m_log->Write("%s", m_buf);
 send(m_client, m_buf, strlen(m_buf), 0);


 // 先检查redis数据库，优先从数据库获取cgi程序的输出
 if (g_redis->HasHash(hkey, hfiled))
 {
  string hcgi = g_redis->GetHash(hkey, hfiled);

  m_log->Write("%s", hcgi.c_str());
  send(m_client, hcgi.c_str(), hcgi.size(), 0);
  m_log->Write("\n\n");
 }
 else
 {
  // 生成2个管道用于双向通信，创建子进程
  // cgi_output：用于子进程向父进程发送数据
  // cgi_input：用于父进程向子进程发送数据
  if (pipe(cgi_output) < 0) 
  {
   cannot_execute(m_client);
   return;
  }
  if (pipe(cgi_input) < 0) 
  {
   cannot_execute(m_client);
   return;
  }
 /* 不确定是否要记录管道标识符方便清理
  m_pipes[0] = cgi_output[0];
  m_pipes[1] = cgi_output[1];
  m_pipes[2] = cgi_intput[0];
  m_pipes[3] = cgi_intput[1];
 //*/
  if ( (pid = fork()) < 0 ) 
  {
   cannot_execute(m_client);
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
   sprintf(meth_env, "REQUEST_METHOD=%s", m_method);
   putenv(meth_env);
   // GET
   if (strcasecmp(m_method, "GET") == 0) 
   {
    sprintf(query_env, "QUERY_STRING=%s", m_query_string);
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
   execl(m_path, m_path, NULL);
   exit(0);
  } 
  // 父进程执行部分
  else 
  {    
   close(cgi_output[1]);
   close(cgi_input[0]);
   
   string temp(1024, '\0');

 
   if (strcasecmp(m_method, "POST") == 0)
    // 获取http实体部分数据，以字符c作为中介通过管道传给cgi进程
    for (i = 0; i < content_length; i++) 
    {
     c = entity_body[i];
     write(cgi_input[1], &c, 1);
    }

   // 从cgi进程获取程序的输出，传入套接字
   while (read(cgi_output[0], &c, 1) > 0)
   {
    m_log->Write("%c", c);

    send(m_client, &c, 1, 0);

    hvalue += c; // 使用hvalue记录cgi程序的输出
   }

   // 记入redis数据库
   g_redis->SetHash(hkey, hfiled, hvalue);
 
   m_log->Write("\n\n");
   close(cgi_output[0]);
   close(cgi_input[1]);
 
   // 等待子进程执行完毕
   waitpid(pid, &status, 0);
  }
 }
}





/*
 * 直接将html文件内容发给客户端
 * 优先从redis数据库获取数据
 * 数据库未保存html文件，再执行cat函数
 */
void Http::serve_file(int m_client, const char *filename)
{
 int numchars = 1;
 char m_buf[1024];

 // 清空套接字内剩余的首部行
 while ((numchars > 0) && strcmp("\n", m_buf)) 
  numchars = get_line(m_client, m_buf, sizeof(m_buf));

 // 如果redis数据库已存有数据,就从数据库中获取html文件
 string sfile(filename);
 if (g_redis->HasString(sfile))
 {
  // 发送http回应报文   
  headers(m_client, filename);
  
  // 获取并发送html文件
  string shtml = g_redis->GetString(sfile);
  m_log->Write("%s", shtml.c_str());
  send(m_client, shtml.c_str(), shtml.size(), 0);
  m_log->Write("\n\n");
 }
 else // 否则执行cat函数获取文件内容
  cat(m_client, filename);
 fclose(m_htmlfp);
}





/* 
 * 获取文件的所有内容，功能与cat命令相同
 * 发送完html文件后，将文件内容存入redis数据库
 */
void Http::cat(int m_client, const char *filename)
{
 char m_buf[1024] = { 0 };
 string svalue, stemp;

 // 打开文件，如果文件存在，就读取文件
 m_htmlfp = fopen(filename, "r");
 if (m_htmlfp == NULL)
  not_found(m_client);
 else
 {
  // 读取的文件长度为1023，确保最后一个字符是'\0'
  fgets(m_buf, sizeof(m_buf) - 1, m_htmlfp);
  while (!feof(m_htmlfp))
  {
   // 写日志
   m_log->Write("%s", m_buf);

   // 发往套接字
   send(m_client, m_buf, strlen(m_buf), 0);

   // 存入redis数据库
   stemp = m_buf;
   svalue += stemp;

   fgets(m_buf, sizeof(m_buf), m_htmlfp);
  }

  // 存入redis数据库
  g_redis->SetString(filename, svalue);

  m_log->Write("\n\n");
 }
}





// http状态行 
void Http::headers(int m_client, const char *filename)
{
 char m_buf[1024];

 m_log->Write("---------- serve_file ---------- \n");
 strcpy(m_buf, "HTTP/1.0 200 OK\r\n"
  SERVER_STRING
  "Content-Type: text/html\r\n"
  "\r\n");

 m_log->Write("%s\n", m_buf);
 send(m_client, m_buf, strlen(m_buf), 0);
}






// 普通的http响应报文
void Http::bad_request(int m_client)
{
 char m_buf[1024];

 sprintf(m_buf, "HTTP/1.0 400 BAD REQUEST\r\n"
  "Content-type: text/html\r\n"
  "\r\n"
  "<P>Your browser sent a bad request, "
  "such as a POST without a Content-Length.\r\n");

 m_log->Write("---------- bad_request ---------- \n");
 m_log->Write("%s\n", m_buf);
 send(m_client, m_buf, sizeof(m_buf), 0);
}





// 普通的http响应报文
void Http::cannot_execute(int m_client)
{
 char m_buf[1024];

 sprintf(m_buf, "HTTP/1.0 500 Internal Server Error\r\n"
  "Content-type: text/html\r\n"
  "\r\n"
  "<P>Error prohibited CGI execution.\r\n");

 m_log->Write("---------- cannot_execute ---------- \n");
 m_log->Write("%s\n", m_buf);
 send(m_client, m_buf, strlen(m_buf), 0);
}





// 普通的http响应报文
void Http::not_found(int m_client)
{
 char m_buf[1024];

 sprintf(m_buf, "HTTP/1.0 404 NOT FOUND\r\n"
  SERVER_STRING
  "Content-Type: text/html\r\n"
  "\r\n"
  "<HTML><TITLE>Not Found</TITLE>\r\n"
  "<BODY><P>The server could not fulfill\r\n"
  "your request because the resource specified\r\n"
  "is unavailable or nonexistent.\r\n"
  "</BODY></HTML>\r\n");

 m_log->Write("---------- not_found ---------- \n");
 m_log->Write("%s\n", m_buf);
 send(m_client, m_buf, strlen(m_buf), 0);
}





// 普通的http响应报文
void Http::unimplemented(int m_client)
{
 char m_buf[1024];

 sprintf(m_buf, "HTTP/1.0 501 Method Not Implemented\r\n"
  SERVER_STRING
  "Content-Type: text/html\r\n"
  "\r\n"
  "<HTML><HEAD><TITLE>Method Not Implemented\r\n"
  "</TITLE></HEAD>\r\n"
  "<BODY><P>HTTP request method not supported.\r\n"
  "</BODY></HTML>\r\n");

 m_log->Write("---------- unimplemented ---------- \n");
 m_log->Write("%s\n", m_buf);
 send(m_client, m_buf, strlen(m_buf), 0);
}





// 发送错误信息
void Http::error_die(const char *sc)
{
 perror(sc);
 exit(1);
}





/* 
 * server.cpp中的线程清理函数的参数为一个Http类的指针，
 * 通过该指针调用Http对象的shutdown函数，关闭可能未关闭的日志文件、html文件。
 */
void Http::shutdown(void* arg)
{
 m_log->Close();
 if (m_htmlfp != NULL)
 {
  fclose(m_htmlfp);
  m_htmlfp;
 }
/*
 for(int i = 0; i < 4; ++i)
  close(m_pipes[i]);
//*/

}
