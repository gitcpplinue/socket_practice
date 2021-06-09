#include "TcpClient.h"

TcpClient::TcpClient()
{ Init(); }

TcpClient::~TcpClient()
{ 
 close(m_sockfd);
}



bool TcpClient::Init()
{
 m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if(m_sockfd == -1)
 {
  perror("socket");
  return false;
 }

 return true;
}


bool TcpClient::Connect(const char *addr, int port)
{
 sockaddr_in servaddr;
 hostent* h = gethostbyname(addr);
 if(h == 0)
 {
  printf("gethostbyname failed.\n");
  close(m_sockfd);
  return false;
 }

 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET; // 地址族
 servaddr.sin_port = htons(port); // 端口号，转换为网络字节序
 memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);
 // servaddr.sin_addr.s_addr = inet_addr("192.168.190.134");

 // 连接到服务端
 if (connect(m_sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
 {
  perror("conncet");
  return false;
 }
 printf("已连接到服务端(%s) \n", inet_ntoa(servaddr.sin_addr));

}


int TcpClient::Send(const void* buf, size_t len, int flags)
{
 return send(m_sockfd, buf, len, flags);
}


int TcpClient::Recv(void* buf, size_t len, int flags)
{
 return recv(m_sockfd, buf, len, flags);
}


