# socket_practice
## 概述
简单的多线程回声服务器
* TcpServer类：封装服务端相关的socket函数。
* TcpClient类：封装客户端相关的socket函数。
* Epoll类：封装epoll函数。
* Log类：提供函数将数据写入日志文件./log/log.txt，使用互斥锁进行线程同步。
* Timer类：计时器。
* SIG.h：定义了一些宏简化代码。

