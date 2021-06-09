#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#include "TcpServer.h"
#include "SIG.h"


void* ClientThread(void* arg); 

void EXIT(int sig);
void th_exit(void* arg);


TcpServer g_tcps;
std::vector<pthread_t> g_vthreads;


int main(int argc, char *argv[])
{
 if (argc != 2)
 {
  printf("Error! try:./server 5000 \n\n");
  return -1;
 }

 // 在套接字完成初始化前屏蔽所有信号
 SIG_DISABLE_ALL;

 g_tcps.Init(atoi(argv[1]));

 // 只设置SIGINT和SIGTERM的信号处理函数
 SIG_SET_FUNC(SIGINT, EXIT);
 SIG_SET_FUNC(SIGTERM, EXIT);

 int count = 0;
 while(1)
 {
  if(g_tcps.Accept() == false)
   continue;

  pthread_t tid;
  int err, clientfd;
  
  clientfd = g_tcps.GetClient();
  err = pthread_create(&tid, NULL, ClientThread, (void*)(long)clientfd);
  if(err != 0)
    printf("线程创建失败，错误值：%d \n", err);
  g_vthreads.push_back(tid); // 将刚申请的客户端套接字存入容器

  printf("第%d个客户端已连接 \n", ++count);
 }
}

// 线程函数
void* ClientThread(void* arg)
{
 int clientfd = (long)arg;

 // 登记线程清理函数
 pthread_cleanup_push(th_exit, (void*)(long)clientfd);

 char buffer[1024];

 while(1)
 {
  int iret;
  memset(buffer, 0, sizeof(buffer));
  iret = g_tcps.Recv(buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
   printf("iret = %d \n", iret);
   break;
  }
  printf("接收: %s \n", buffer);
  

  iret = g_tcps.Send(buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  printf("发送: %s \n", buffer);
  printf("----------------------------\n");
 }

 printf("客户端已断开连接\n");
 close(clientfd);

 // 线程正常结束，弹出清理函数而不执行
 pthread_cleanup_pop(0);

 printf("线程%x正常退出 \n", pthread_self());
 pthread_exit(0);
}



// -------------------------清理函数-----------------------
// 退出信号处理函数
void EXIT(int sig)
{
 if(sig > 0) 
 { // 屏蔽信号，避免在运行的过程中再被调用
  signal(sig, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
 }

// 终止所有线程
 for(pthread_t tid: g_vthreads) 
   pthread_cancel(tid);

 sleep(3); // 给线程的清理留出时间
 printf("服务端收到信号终止 \n");
 
 close(g_tcps.GetListen());
 close(g_tcps.GetClient());
  
 exit(0);
}

// 线程清理函数
void th_exit(void* arg)
{
 int socket = (long)arg;
 // 目前需要清理的资源只有客户端套接字
 close(socket);
 printf("线程%x已关闭套接字%d \n", pthread_self(), socket);
}
