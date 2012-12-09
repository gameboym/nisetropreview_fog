// 日本語

#ifdef _WIN32
	#include "cthread.win.h"
#elif defined(_LINUX)
	#include "cthread.linux.h"
#endif


#ifndef __CTHREAD_H__
#define __CTHREAD_H__

// デフォルトの待機時間
#ifndef CTHREAD_DEFAULT_WAITTIME
#define CTHREAD_DEFAULT_WAITTIME
const int __cthread_waittime = 500;
#endif

/*!
 * @brief	スレッド用クラス
 * */
#ifdef _WIN32
typedef CTHREAD_WIN CTHREAD;
#elif defined(_LINUX)
typedef CTHREAD_LINUX CTHREAD;
#endif


#endif


