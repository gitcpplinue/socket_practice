#pragma once

#include <sys/time.h>
#include <time.h>
#include <stdio.h>


enum timeState{ running, pause, stop };

class Timer
{
private:
  double m_begin, m_end, m_count;
  timeState m_state;

  // 将timeval格式数据转换为double
  double Time2Double(timeval time);
public:
  Timer();
  ~Timer();
  
  // 成功返回0，失败返回-1
  int Start();
  // 成功返回计时时间，失败返回-1
  int Pause(); 
  // 成功返回计时时间并将m_count清零，失败返回-1
  int Stop();
};

// 以“yyyy-mm-dd hh:mm:ss”的格式返回当前时间
string LocalTime();
