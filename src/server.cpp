#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TcpServer.h"
#include "SIG.h"

void EXIT(int sig);


TcpServer tcps;


int main(int argc, char *argv[])
{
 if (argc != 2)
 {
  printf("Error! try:./server 5000 \n\n");
  return -1;
 }

 // 在套接字完成初始化前屏蔽所有信号
 SIG_DISABLE_ALL;

 tcps.Init(atoi(argv[1]));

 // 只设置SIGINT和SIGTERM的信号处理函数
 SIG_SET_FUNC(SIGINT, EXIT);
 SIG_SET_FUNC(SIGTERM, EXIT);


 tcps.Accept();

 printf("等待5s,尝试将监听套接字关闭后再与客户端通信 \n");
 close(tcps.GetListen());
 sleep(5);

 char buffer[1024];
 while(1)
 {
  int iret;
  memset(buffer, 0, sizeof(buffer));
  iret = tcps.Recv(buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
   printf("iret = %d \n", iret);
   break;
  }
  printf("接收: %s \n", buffer);
  

  iret = tcps.Send(buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  printf("发送: %s \n", buffer);
  printf("----------------------------\n");
 }

}




void EXIT(int sig)
{
 if(sig > 0) 
 { // 屏蔽信号，避免在运行的过程中再被调用
  signal(sig, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
 }
 
 printf("服务端收到信号终止 \n");
 
 close(tcps.GetListen());
 close(tcps.GetClient());
  
 exit(0);
}




