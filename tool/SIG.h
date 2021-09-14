#pragma once
#include <signal.h>
#include <stdio.h>

#ifndef __SIG_H__
#define __SIG_H__

// 禁用所有信号
#define SIG_DISABLE(i) signal(i, SIG_IGN)
#define SIG_DISABLE_ALL for(int i = 1; i <=31; ++i) SIG_DISABLE(i) 

// 启用信号i
#define SIG_ENABLE(i) signal(i, SIG_DEF)
#define SIG_ENABLE_ALL for(int i = 1; i <=31; ++i) SIG_ENABLE(i) 

// 设置信号i的处理函数
#define SIG_SET_FUNC(i, foo) signal(i, foo)

#endif






