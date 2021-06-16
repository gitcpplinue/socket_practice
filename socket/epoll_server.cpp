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
#include "Epoll.h"


#define TCPBUFFER 1024


// TCP保活机制激活与设置
int SetKeepAlive(int fd, int time, int intvl, int probes);

// 响应终止信号9和15
void EXIT(int sig);


TcpServer g_tcps;
Epoll g_epoll;

int main(int argc, char *argv[])
{
 if (argc != 2)
 {
  printf("Error! try:./server 5000 \n\n");
  return -1;
 }

 int count = 0;
 int listenfd;
 char tcpbuffer[TCPBUFFER];

 // 在套接字完成初始化前屏蔽所有信号
 SIG_DISABLE_ALL;

 g_tcps.Init(atoi(argv[1]));

 // 只设置SIGINT和SIGTERM的信号处理函数
 SIG_SET_FUNC(SIGINT, EXIT);
 SIG_SET_FUNC(SIGTERM, EXIT);


 listenfd = g_tcps.GetListen();
 // 创建epoll，加入监听套接字
 g_epoll.Create();
 g_epoll.Add(listenfd, EPOLLIN);

 while(1)
 {
  int indfd = g_epoll.Wait();
  if(indfd == 0)
  {
   printf("epoll_wait timeout \n");
   continue;
  }
  
  for(int i = 0; i < indfd; ++i)
  {
   epoll_event tmp = g_epoll.GetEvent(i);

   // 如果是监听套接字响应，调用accept，将新的客户端套接字存入epoll
   if((tmp.data.fd == listenfd) && (tmp.events & EPOLLIN))
   {
    if(g_tcps.Accept() == false)
     continue;

    int clientfd = g_tcps.GetClient();
    g_epoll.Add(clientfd, EPOLLIN);

    printf("第%d个客户端已连接 \n", ++count);
    SetKeepAlive(clientfd, 120, 20, 5);

   }
   else if(tmp.events & EPOLLIN) // 默认其它响应EPOLLIN事件的套接字都是客户端套接字
   { // 将客户端发送的消息重新发送回去
    int iret;

    // ----------接收-----------
    memset(tcpbuffer, 0, sizeof(tcpbuffer));
    iret = g_tcps.Recv(tmp.data.fd, tcpbuffer, sizeof(tcpbuffer), 0);

    if(iret <= 0) // tcp连接的另一端发生错误或已断开
    {
     g_epoll.Delete(tmp.data.fd, EPOLLIN);
     printf("套接字%d连接的客户端已断开\n", tmp.data.fd);
     close(tmp.data.fd);  
     continue;
    }
    printf("接收: %s \n", tcpbuffer);


    // ----------发送-----------
    iret = g_tcps.Send(tmp.data.fd, tcpbuffer, strlen(tcpbuffer), 0);
    if(iret <= 0)
    {
     g_epoll.Delete(tmp.data.fd, EPOLLIN);
     printf("套接字%d连接的客户端已断开\n", tmp.data.fd);
     close(tmp.data.fd);  
     continue;
    }
    printf("发送: %s \n", tcpbuffer);
    printf("----------------------------\n");
   }
  }
 }
 return 0;
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


 printf("服务端收到信号终止 \n");
 
 close(g_tcps.GetListen());
 close(g_tcps.GetClient()); // close调用错误会返回-1，errno设为9:Bad file descriptor
  
 exit(0);
}

