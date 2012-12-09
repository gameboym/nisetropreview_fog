// “ú–{Œê
#include "cmutex.h"

CMUTEX_WIN::CMUTEX_WIN()
{
	printf( "mutex win Constructor\n" );
	m_hMutex = NULL;
	this->create();
}

CMUTEX_WIN::~CMUTEX_WIN()
{
	printf( "mutex win Destructor\n" );
	this->destroy();
}

void CMUTEX_WIN::create( void )
{
	printf( "mutex win create\n" );
	TCHAR buf[256];
#if _MSC_VER >= 1400
	_stprintf_s( buf , TEXT("mutex%d") , (int)this );
#else
	_stprintf( buf , TEXT("mutex%d") , (int)this );
#endif
	if( !m_hMutex ){
		m_hMutex = OpenMutex( 0 , FALSE , buf);
		if( m_hMutex ) return;

		m_hMutex = CreateMutex(NULL, false, buf);
	}
}

void CMUTEX_WIN::destroy( void )
{
	printf( "mutex win destroy\n" );
	if( m_hMutex )
	{
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
}
int CMUTEX_WIN::lock( DWORD dwMSec )
{
//	if(WaitForMultipleObjects( 1 , &m_hMutex , true , 10 ) != WAIT_OBJECT_0)
	if(WaitForSingleObject(m_hMutex, dwMSec) != WAIT_OBJECT_0)
		return true;

//	printf( "mutex win lock\n" );
	return false;
}

int CMUTEX_WIN::unlock( void )
{
//	printf( "mutex win unlock\n" );
	if( !m_hMutex )
		return true;

	ReleaseMutex(m_hMutex);
	return false;
}



