// 日本語
#include "cthread.h"

CTHREAD_WIN::CTHREAD_WIN()
{
	printf( "thread windows Constructor\n" );

	m_hThread = NULL;
	m_bWork = false;
	m_iWait = __cthread_waittime;
	//
}

CTHREAD_WIN::~CTHREAD_WIN()
{
	printf( "thread windows Destructor\n" );
	if(IsWork())
		stopThread();
	//
}


void CTHREAD_WIN::startThread( void )
{
	printf( "thread windows start\n" );
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, this->thread_func, (void*)this, 0, NULL);
	m_bWork = true;
}

void CTHREAD_WIN::stopThread( void )
{
	printf( "thread windows stop\n" );
	m_bWork = false;
	WaitForSingleObject(m_hThread, INFINITE);
	if( m_hThread )
	{
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}
}

// スレッド用関数
unsigned int __stdcall CTHREAD_WIN::thread_func( void* pData )
{
	CTHREAD_WIN* pThread = (CTHREAD_WIN*)pData;

	while( pThread->IsWork() )
	{
		pThread->ThreadFunction();
		Sleep(pThread->getSleepTime());
	}
	printf( "thread windows _endthread(%d)\n" , (int)pThread );
	_endthread();

	return 0;
}
