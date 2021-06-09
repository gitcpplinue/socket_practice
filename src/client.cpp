#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TcpClient.h"
#include "SIG.h"

void EXIT(int sig);


TcpClient tcpc;

int main(int argc, char *argv[])
{
 if (argc != 3)
 {
  printf("Error! try:./server 127.0.0.1 5000 \n\n");
  return -1;
 }


 SIG_DISABLE_ALL;

 tcpc.Init();

 SIG_SET_FUNC(SIGINT, EXIT);
 SIG_SET_FUNC(SIGTERM, EXIT);


 tcpc.Connect(argv[1], atoi(argv[2]));

 char buffer[1024];
 for(int i = 0; i < 10; ++i)
 {
  sleep(1);

  int iret;
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer, "第%d条消息，编号%03d。", i + 1 ,i + 1);

  iret = tcpc.Send(buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  printf("发送：%s \n", buffer);


  memset(buffer, 0, sizeof(buffer));
  iret = tcpc.Recv(buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
   printf("iret = %d \n", iret);
   break;
  }
  printf("接收：%s \n", buffer);
  printf("--------------------------\n");

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

 printf("客户端收到信号终止 \n");

 close(tcpc.GetSocket());

 exit(0);
}

