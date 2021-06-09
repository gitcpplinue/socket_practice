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


int main(int argc, char *argv[])
{
 if (argc != 2)
 {
  printf("Error! try:./server 5000 \n\n");
  return -1;
 }

 TcpServer tcps(atoi(argv[1]));

 tcps.Accept();

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
