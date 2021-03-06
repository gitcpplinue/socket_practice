#include "TcpServer.h"

TcpServer::TcpServer()
{
 m_listenfd = 0;
 m_clientfd = 0; 
}

TcpServer::~TcpServer()
{ 
 if(m_listenfd != 0)
 {
  close(m_listenfd);
  m_listenfd = 0;
 }

 // 似乎对已经关闭的套接字标识符再次调用close()不会出大问题，所以暂时这么处理
 if(m_clientfd != 0)
 {
  close(m_clientfd);
  m_clientfd = 0; 
 }
}



bool TcpServer::Init(int port)
{
 m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
 if(m_listenfd == -1)
 {
  perror("socket");
  return false;
 }
 // 监听套接字参数初始化
 sockaddr_in servaddr;
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_port = htons(port);
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

 // 绑定
 if(bind(m_listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
 {
  perror("bind");
  return false;
 }
 
 // 监听
 if(listen(m_listenfd, 10) != 0)
 {
  perror("listen");
  return false;
 }

 printf("等待客户端连接\n");

 return true;
}


bool TcpServer::Accept()
{
 // 在这里不需要对客户端的信息进行操作，后2个参数填0
 m_clientfd = accept(m_listenfd, NULL, NULL); 
 if(m_clientfd == -1)
 {
  perror("accept");
  return false;
 }
 return true;
}


int TcpServer::Send(const void* buf, size_t len, int flags)
{
 return send(m_clientfd, buf, len, flags);
}
int TcpServer::Send(int sockfd, const void* buf, size_t len, int flags)
{
 return send(sockfd, buf, len, flags);
}


int TcpServer::Recv(void* buf, size_t len, int flags)
{
 return recv(m_clientfd, buf, len, flags);
}
int TcpServer::Recv(int sockfd, void* buf, size_t len, int flags)
{
 return recv(sockfd, buf, len, flags);
}

 

