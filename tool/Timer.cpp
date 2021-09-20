#include "Timer.h"

Timer::Timer()
{
 m_begin = m_end = m_count = 0;
 m_state = STOP;
}

Timer::~Timer()
{}


// 使用m_begin记下开始的时间，状态改为RUN
double Timer::Start()
{
 if(m_state != RUN)  
 {
  timeval begin;

  gettimeofday(&begin, 0); 
  m_begin = Time2Double(begin);
  m_state = RUN;

  return 0;
 }
 else
  return -1;
}

// 只有当计时器状态为RUN时才能正确执行
// 用m_end记录当前时间，返回m_end-m_begin的差值，状态改为PAUSE 
double Timer::Pause()
{
 if(m_state == RUN)  
 { 
  timeval end;

  gettimeofday(&end, 0); 
  m_end = Time2Double(end);
  m_count += m_end - m_begin;
  m_state = PAUSE;

  return m_count;
 }
 else
  return -1;
}

// 如果处于RUN状态，计算新的时间差值
// 最后返回m_count的值，将m_count清空 
double Timer::Stop()
{
 if(m_state == STOP)  
  return -1;

 if(m_state == RUN)
 { 
  timeval end;

  gettimeofday(&end, 0); 
  m_end = Time2Double(end);
  m_count += m_end - m_begin;
  m_state = STOP;
 }

  double tmp = m_count;
  m_count = 0;
  return tmp;
}


// 格式化返回当前时间
string LocalTime()
{
 time_t cur_t;
 tm *tmp;
 char buf[50];

 memset(buf, 0, sizeof(buf));

 time(&cur_t);
 tmp = localtime(&cur_t);
 strftime(buf, 50, "[%F %X]", tmp); 
 
 string ret = buf;
 return ret;
}



// 同上
string Date()
{
 time_t cur_t;
 tm *tmp;
 char buf[50];

 memset(buf, 0, sizeof(buf));

 time(&cur_t);
 tmp = localtime(&cur_t);
 strftime(buf, 50, "%F", tmp); 
 
 string ret = buf;
 return ret;
}

string Time()
{
 time_t cur_t;
 tm *tmp;
 char buf[50];

 memset(buf, 0, sizeof(buf));

 time(&cur_t);
 tmp = localtime(&cur_t);
 strftime(buf, 50, "%X", tmp); 
 
 string ret = buf;
 return ret;
}



double Timer::Time2Double(timeval time)
{
  char tmp[20]; 
  memset(tmp, 0, 20);
  sprintf(tmp, "%ld.%ld", time.tv_sec, time.tv_usec);
  return atof(tmp);
}


