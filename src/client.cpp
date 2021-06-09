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

int main(int argc, char *argv[])
{
 if (argc != 3)
 {
  printf("Error! try:./server 127.0.0.1 5000 \n\n");
  return -1;
 }


 // 创建套接字
 int sockfd; 
 sockfd = socket(AF_INET, SOCK_STREAM, 0); // 地址族、socket类型、默认选项
 if (sockfd == -1)
 {
  perror("socket");
  return -1;
 }



 // 设置套接字相关参数
 sockaddr_in servaddr;
 hostent* h = gethostbyname(argv[1]);
 if(h == 0)
 {
  printf("gethostbyname failed.\n");
  close(sockfd);
  return -1;
 }



 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET; // 地址族 
 servaddr.sin_port = htons(atoi(argv[2])); // 端口号，转换为网络字节序
 memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
 // servaddr.sin_addr.s_addr = inet_addr("192.168.190.134");

 // 连接到服务端 
 if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
 {
  perror("conncet");
  return -1;
 }
 printf("已连接到服务端(%s) \n", inet_ntoa(servaddr.sin_addr));

 char buffer[1024];
 for(int i = 0; i < 10; ++i)
 {
  int iret;
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer, "第%d条消息，编号%03d。", i + 1 ,i + 1);

  iret = send(sockfd, buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  printf("发送：%s \n", buffer);


  memset(buffer, 0, sizeof(buffer));
  iret = recv(sockfd, buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
   printf("iret = %d \n", iret);
   break;
  }
  printf("接收：%s \n", buffer);
  printf("--------------------------\n");

 }
 close(sockfd);
}
