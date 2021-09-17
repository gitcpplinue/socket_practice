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
 redisContext *m_connect;
 redisReply *m_reply;
 pthread_mutex_t m_mutex;

 void PrintError(string, redisContext* = NULL, redisReply* = NULL);

public:
 Redis();
 ~Redis();

 int Connect();

 int SetString(string key, string value);
 string GetString(string key);
 bool HasString(string key);

 int SetHash(string key, string filed, string value);
 string GetHash(string key, string filed);
 bool HasHash(string key, string filed);

};

extern Redis g_redis;

