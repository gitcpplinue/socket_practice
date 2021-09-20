#pragma once
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include <hiredis/hiredis.h>

using namespace std;

class Redis
{
private:
 redisContext *m_connect;	// redis连接
 redisReply *m_reply;		// redis回应
 pthread_mutex_t m_mutex;	// 互斥锁

 // 输出错误，并释放redis回应、连接
 void PrintError(string, redisContext* = NULL, redisReply* = NULL);

public:
 Redis();
 ~Redis();

 // 连接redis数据库
 int Connect();

 // 设置string类型数据，出错返回-1
 int SetString(string key, string value);
 // 获取redis类型数据
 string GetString(string key);
 // 查询redis数据库中是否存有key对应的string类型数据
 bool HasString(string key);

 // 同上
 int SetHash(string key, string filed, string value);
 string GetHash(string key, string filed);
 bool HasHash(string key, string filed);

};

extern Redis *g_redis;
//extern Redis g_redis;

