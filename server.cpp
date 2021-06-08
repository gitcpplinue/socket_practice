#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
 if (argc != 2)
 {
  printf("Error! try:./server 5000 \n\n");
  return -1;
 }


 // 创建套接字
 int listenfd; // 监听套接字
 listenfd = socket(AF_INET, SOCK_STREAM, 0); // 地址族、socket类型、默认选项
 if (listenfd == -1)
 {
  perror("socket");
  return -1;
 }

 // 设置监听套接字的相关参数
 sockaddr_in servaddr;
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET; // 地址族 
 servaddr.sin_port = htons(atoi(argv[1])); // 端口号，转换为网络字节序
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // IP地址, INADDR_ANY对应0.0.0.0，表示任何地址

 // 关联地址和监听套接字
 if (bind(listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
 {
  perror("bind");
  return -1;
 }

 // 调用listen函数来宣告服务端愿意接受连接请求
 if(listen(listenfd, 5) != 0)
 {
  perror("listen");
  return -1;
 }
 printf("等待客户端连接\n");

 int clientfd;
 int socklen = sizeof(sockaddr_in);
 sockaddr_in clientaddr;

 // 新的套接字clientfd与原始套接字listenfd具有相同的套接字类型和地址族
 clientfd = accept(listenfd, (sockaddr*)&clientaddr, (socklen_t*)&socklen);

 // 将网络字节序的二进制地址转换成文本字符串
 printf("客户端(%s)已连接。\n", inet_ntoa(clientaddr.sin_addr));


 char buffer[1024];
 while(1)
 {
  int iret;
  memset(buffer, 0, sizeof(buffer));
  iret = recv(clientfd, buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
   printf("iret = %d \n", iret);
   break;
  }
  printf("接收: %s \n", buffer);
  

  iret = send(clientfd, buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  printf("发送: %s \n", buffer);
  printf("----------------------------\n");
 }

 close(listenfd);
 close(clientfd);
}
