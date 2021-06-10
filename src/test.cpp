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


#define NUM 5120

void* th_foo(void* arg);

int main(int argc, char *argv[])
{
 int sockets[NUM] = { 0 };

 sockaddr_in servaddr;
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_port = htons(5000);
 servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

 for(int i = 0; i < NUM; ++i)
 {
  sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
  
  if(sockets[i] == -1)
  {
   printf("%d",i); perror("socket");
   break;
  }
  
  if(connect(sockets[i], (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
  {
   printf("%d",i); perror("connect");
   break;
  }

  printf("套接字%d已连接到服务端\n", sockets[i]);
 } 
 
}
