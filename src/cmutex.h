// 日本語

#ifdef _WIN32
#include "cmutex.win.h"
#elif defined(_LINUX)
#include "cmutex.linux.h"
#endif


#ifndef __CMUTEX_H__
#define __CMUTEX_H__

/*!
 * @brief	ミューテクス用クラス
 * */
#ifdef _WIN32
typedef CMUTEX_WIN CMUTEX;
#elif defined(_LINUX)
typedef CMUTEX_LINUX CMUTEX;
#endif

#endif


