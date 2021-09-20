# socket_practice






# linux

将原来的回声服务器改为web服务器。

web服务相关的代码来自项目`Tinyhttpd`（ https://sourceforge.net/projects/tiny-httpd/ ）。在原基础上将其封装为http类，增加了redis缓存和日志。



## 运行

```
make server
./server (要运行的端口号)
```

1. 运行服务端时要启动Redis服务器，否则会报错。

2. 如果要从运行服务端的虚拟机以外访问服务器，得先让防火墙开放相应端口。例如：

   ```
   firewall-cmd --add-port=5000/tcp
   ```

3. 注意htdocs目录下cgi文件中第一行指定的perl程序路径。路径错误就无法运行cgi程序。



## 目录结构

```
[root@localhost socket_practice]# tree
.
├── htdocs
│   ├── check.cgi
│   ├── color.cgi
│   ├── index.html
│   └── README
├── log
│   ├── log.txt
│   └── web
│       └── 210918
├── makefile
├── README.md
├── server
├── socket
│   ├── client.cpp
│   ├── Epoll.cpp
│   ├── Epoll.h
│   ├── epoll_server.cpp
│   ├── http.cpp
│   ├── http.h
│   ├── server.cpp
│   ├── TcpClient.cpp
│   ├── TcpClient.h
│   ├── TcpServer.cpp
│   └── TcpServer.h
├── tool
│   ├── Log.cpp
│   ├── Log.h
│   ├── Redis.cpp
│   ├── Redis.h
│   ├── SIG.h
│   ├── Timer.cpp
│   └── Timer.h
└── urls.txt
```



## **htdocs**

存放提供web服务所需的网页文件。



## **log**

存放服务端运行过程中生成的日志。



## **socket**

存放与网络通信相关的源代码。

**client.cpp**

原回声服务器的客户端。

**Epoll.h、Epoll.cpp**

封装了epoll操作。

**epoll_server.cpp**

使用epoll实现多线程的回声服务器。

**http.h、http.cpp**

将`Tinyhttpd`的源代码封装为http类。

**server.cpp**

在原回声服务器基础上修改的服务端代码。

**TcpClient.h、TcpClient.cpp**

封装客户端的socket函数。

**TcpServer.h、TcpServer.cpp**

封装服务端的socket函数。


## tool

存放一些工具类。

**Log.h、Log.cpp**

封装日志类的操作。

**Redis.h、Redis.cpp**

封装连接Redis和存取数据的操作。

**Timer.h、Timer.cpp**

封装计时器类，并提供了获取当前时间的函数。



