#include "Redis.h"



Redis *g_redis;
//Redis g_redis;

Redis::Redis()
{
 m_connect = NULL;
 m_reply = NULL;
 pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
}




Redis::~Redis()
{
 if (m_reply != NULL)
 {
  freeReplyObject(m_reply);
  m_reply = NULL;
 }

 if (m_connect != NULL)
 {
  redisFree(m_connect);
  m_connect = NULL;
 }

 pthread_mutex_unlock(&m_mutex);
 pthread_mutex_destroy(&m_mutex);
}


// 输出错误
void Redis::PrintError(string str, redisContext* con, redisReply* rep)
{
 printf("%s\n", str.c_str());
 if (con != NULL)
  redisFree(con);
 if (rep != NULL)
  freeReplyObject(m_reply);
}


// 连接数据库
int Redis::Connect()
{
 m_connect = redisConnect("127.0.0.1", 6379);
 if (m_connect->err)
 {
  PrintError("Redis Connect Failed", m_connect);
  return -1;
 }
 printf("Redis Connected\n");
}


// 设置字符串类型数据
int Redis::SetString(string key, string value)
{
 pthread_mutex_lock(&m_mutex);

 m_reply = (redisReply*)redisCommand(m_connect, "SET %s %b", 
	key.c_str(), value.c_str(), value.size());
 if (m_reply == NULL)
 {
  PrintError("SetObject Failed", m_connect);
  pthread_mutex_unlock(&m_mutex);
  return -1;
 }

 if (!(m_reply->type == REDIS_REPLY_STATUS && strcasecmp(m_reply->str, "OK")) == 0)
 {
  printf("SetObject Failed\n", m_connect, m_reply);
  pthread_mutex_unlock(&m_mutex);
  return -1;
 }
 freeReplyObject(m_reply);
 printf("Object set OK\n");

 pthread_mutex_unlock(&m_mutex);
}


// 获取字符串类型数据
string Redis::GetString(string key)
{
 pthread_mutex_lock(&m_mutex);

 m_reply = (redisReply*)redisCommand(m_connect, "GET %s", key.c_str());
 if (m_reply->type != REDIS_REPLY_STRING)
 {
  PrintError("GetObject Failed", m_connect, m_reply);
  pthread_mutex_unlock(&m_mutex);
  return "error";
 }

 string sret(m_reply->str);

 freeReplyObject(m_reply);

 printf("Object get OK\n");

 pthread_mutex_unlock(&m_mutex);
 return sret; 
}


// 检查redis数据库中是否存有string类型数据key
bool Redis::HasString(string key)
{
 pthread_mutex_lock(&m_mutex);

 bool set = true;
 m_reply = (redisReply*)redisCommand(m_connect, "GET %s", key.c_str());

 if (m_reply->type == REDIS_REPLY_NIL)
  set = false;
 freeReplyObject(m_reply);

 pthread_mutex_unlock(&m_mutex);

 return set;
}




int Redis::SetHash(string key, string filed, string value)
{
 pthread_mutex_lock(&m_mutex);

 m_reply = (redisReply*)redisCommand(m_connect, "HSET %s %s %b",
        key.c_str(), filed.c_str(), value.c_str(), value.size());
 if (m_reply == NULL)
 {
  PrintError("SetObject Failed", m_connect);
  pthread_mutex_unlock(&m_mutex);
  return -1;
 }

 if (!(m_reply->type == REDIS_REPLY_STATUS && strcasecmp(m_reply->str, "1")) == 0)
 {
  printf("SetObject Failed\n", m_connect, m_reply);
  pthread_mutex_unlock(&m_mutex);
  return -1;
 }
 freeReplyObject(m_reply);
 printf("Object set OK\n");

 pthread_mutex_unlock(&m_mutex);
}


string Redis::GetHash(string key, string filed)
{
 pthread_mutex_lock(&m_mutex);
 
 m_reply = (redisReply*)redisCommand(m_connect, "HGET %s %s", key.c_str(), filed.c_str());
 if (m_reply->type != REDIS_REPLY_STRING)
 {
  PrintError("GetObject Failed", m_connect, m_reply);
  pthread_mutex_unlock(&m_mutex);
  return "error";
 }
 
 string sret(m_reply->str);
 
 freeReplyObject(m_reply);
 
 printf("Object get OK\n");
 
 pthread_mutex_unlock(&m_mutex);
 return sret;

}




// 检查redis数据库中是否存有hash类型数据key-filed
bool Redis::HasHash(string key, string filed)
{
 pthread_mutex_lock(&m_mutex);

 bool set = true;
 m_reply = (redisReply*)redisCommand(m_connect, "HGET %s %s", key.c_str(), filed.c_str());

 if (m_reply->type == REDIS_REPLY_NIL)
  set = false;
 freeReplyObject(m_reply);

 pthread_mutex_unlock(&m_mutex);

 return set;
}




