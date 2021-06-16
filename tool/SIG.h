#pragma once
#include <signal.h>
#include <stdio.h>

#ifndef __SIG_H__
#define __SIG_H__

#define SIG_DISABLE(i) signal(i, SIG_IGN)
#define SIG_DISABLE_ALL for(int i = 1; i <=31; ++i) SIG_DISABLE(i) 

#define SIG_ENABLE(i) signal(i, SIG_DEF)
#define SIG_ENABLE_ALL for(int i = 1; i <=31; ++i) SIG_ENABLE(i) 

#define SIG_SET_FUNC(i, foo) signal(i, foo)

#endif






