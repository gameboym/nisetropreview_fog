// ���{��

#ifdef _WIN32
	#include "cthread.win.h"
#elif defined(_LINUX)
	#include "cthread.linux.h"
#endif


#ifndef __CTHREAD_H__
#define __CTHREAD_H__

// �f�t�H���g�̑ҋ@����
#ifndef CTHREAD_DEFAULT_WAITTIME
#define CTHREAD_DEFAULT_WAITTIME
const int __cthread_waittime = 500;
#endif

/*!
 * @brief	�X���b�h�p�N���X
 * */
#ifdef _WIN32
typedef CTHREAD_WIN CTHREAD;
#elif defined(_LINUX)
typedef CTHREAD_LINUX CTHREAD;
#endif


#endif


