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
#include <netinet/tcp.h>
#include <vector>

#include "TcpServer.h"
#include "SIG.h"
#include "Log.h"
#include "Timer.h"


#define LOCK(mutex)  pthread_mutex_lock(&mutex) 
#define UNLOCK(mutex) pthread_mutex_unlock(&mutex) 



// 线程主函数
void* ClientThread(void* arg); 


// TCP保活机制激活与设置
int SetKeepAlive(int fd, int time, int intvl, int probes);

// 将sin_addr转换为可读的字符串
void getsockaddr(int sockfd, char* s_addr);


// 响应终止信号9和15
void EXIT(int sig);
// 线程退出函数
void th_exit(void* arg);



TcpServer g_tcps; // 封装了TCP服务端的操作
Log g_log; // 日志
std::vector<pthread_t> g_vthreads; // 存储所有线程id
pthread_mutex_t g_mutex; // 互斥锁


int main(int argc, char *argv[])
{
 if (argc != 2)
 {
  printf("Error! try:./server 5000 \n\n");
  return -1;
 }

 // 在套接字完成初始化前屏蔽所有信号
 SIG_DISABLE_ALL;

 if(g_tcps.Init(atoi(argv[1])) == false) 
  return -1;
 int err =  pthread_mutex_init(&g_mutex, 0);
 if(err != 0)
 {
  printf("mutex_init(%d):%s \n", err, strerror(err));
  return -1;
 }
 g_log.Open("../log/log.txt", "a");

 
 // 只设置SIGINT和SIGTERM的信号处理函数
 SIG_SET_FUNC(SIGINT, EXIT);
 SIG_SET_FUNC(SIGTERM, EXIT);

 int count = 0;
 while(1)
 {
  if(g_tcps.Accept() == false)
  {
   sleep(1);
   continue;
  }

  pthread_t tid;
  int err, clientfd;
  char s_addr[15];
  memset(s_addr, 0, 15);

  
  clientfd = g_tcps.GetClient();
  err = pthread_create(&tid, NULL, ClientThread, (void*)(long)clientfd);
  if(err != 0)
    printf("线程创建失败，错误值：%d \n", err);
  g_vthreads.push_back(tid); // 将新创建的线程id存入容器

  getsockaddr(clientfd, s_addr);
  LOCK(g_mutex);
  g_log.Write("客户端(%s)已连接 \n", s_addr);
  UNLOCK(g_mutex);
 }
 g_log.Close();
}

// 线程函数
void* ClientThread(void* arg)
{
 int clientfd = (long)arg;
 int byte_recv = 0;
 int byte_send = 0;
 Timer timer;

 timer.Start();


 SetKeepAlive(clientfd, 120, 20, 5);

 // 登记线程清理函数
 pthread_cleanup_push(th_exit, (void*)(long)clientfd);

 char buffer[1024];

 // 将收到的数据原封不动发回客户端
 while(1)
 {
  int iret;
  memset(buffer, 0, sizeof(buffer));
  iret = g_tcps.Recv(clientfd, buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
//   printf("iret = %d \n", iret);
   break;
  }
  byte_recv += strlen(buffer);
  printf("接收: %s \n", buffer);
  

  iret = g_tcps.Send(clientfd, buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  byte_send += strlen(buffer);
  printf("发送: %s \n", buffer);
  printf("----------------------------\n");
 }

// printf("客户端已断开连接\n");
 close(clientfd);

 // 线程正常结束，弹出清理函数而不执行
 pthread_cleanup_pop(0);

 double runtime = timer.Stop();
 
 printf("线程%x正常退出 \n", pthread_self());


 char s_addr[15];
 memset(s_addr, 0, 15);
 getsockaddr(clientfd, s_addr);
 LOCK(g_mutex);
 g_log.Write("客户端(%s)已断开连接。接收%dbyte，发送%dbyte，连接持续%.3fs\n", 
	s_addr, byte_recv, byte_send, runtime);
 UNLOCK(g_mutex); 

 pthread_exit(0);
}


// TCP保活机制激活与设置
// time: 经过time秒没发送报文后，执行保活操作
// intvl: 报文段发送间隔
// probes: 尝试次数
int SetKeepAlive(int fd, int time, int intvl, int probes)
{
 int val = 1;
 if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1)
 {
  printf("error:SOL_SOCKET SO_KEEPALIVE \n");
  return -1;
 }

 val = time;
 if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val)) == -1)
 {
  printf("error:IPPROTO_TCP TCP_KEEPIDLE \n");
  return -1;
 }

 val = intvl; 
 if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) == -1)
 {
  printf("error:IPPROTO_TCP TCP_KEEPINTVL \n");
  return -1;
 }
 
 val = probes;
 if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val)) == -1)
 {
  printf("error:IPPROTO_TCP TCP_KEEPCNT \n");
  return -1;
 }

 return 0;
}


void getsockaddr(int sockfd, char* s_addr)
{
 sockaddr_in addr;
 socklen_t len = sizeof(addr);
 getpeername(sockfd, (sockaddr*)&addr, &len);
 sprintf(s_addr, "%s", inet_ntoa(addr.sin_addr));
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
   pthread_cancel(tid); // 对已经停止的线程再调用pthread_cancel，返回值为3

 sleep(1); // 给线程的清理留出时间
 g_log.Write("服务端收到信号终止 \n");
 printf("服务端收到信号终止 \n");

 g_log.Close(); 
 close(g_tcps.GetListen());
 close(g_tcps.GetClient()); // close调用错误会返回-1，errno设为9:Bad file descriptor
  
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
