
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "_tprintd.h"

/*!
	@brief	�G���[�\���p�֐�(�}���`�o�C�g������p)
	@note	�g������_tprintf�Ƃقڈꏏ
*/
void printd( const char * _Format, ... ){
#if defined(_DEBUG)
	va_list	arglist;
	char	*buffer = NULL;
	size_t	len;
	size_t	fmt_len;
	
	va_start(arglist, _Format);
	fmt_len = (int)strlen(_Format) + 1;
	if( fmt_len == 0 )
		return;
	
	len = (size_t)_vscprintf( _Format, arglist )+1;
	if( len < fmt_len ){
		len = fmt_len;
	}
	buffer = malloc( sizeof(char) * len );
	if( buffer ){
#if _MSC_VER >= 1400
		vsprintf_s ( buffer , len , _Format, arglist);
#else
		vsprintf( buffer , _Format, arglist);
#endif
	}
	if( buffer ){
#if defined(_CONSOLE)
		printf( buffer );
#else
		OutputDebugStringA(buffer);
#endif
	}
	if( buffer ){free(buffer); buffer = NULL; }
	va_end(arglist);
#endif
}

/*!
	@brief	�G���[�\���p�֐�(���C�h�o�C�g������p)
	@note	�g������_tprintf�Ƃقڈꏏ
*/
void wprintd( const wchar_t * _Format, ... ){
#if defined(_DEBUG)
	va_list	arglist;
	wchar_t	*buffer = NULL;
	size_t	len;
	size_t	fmt_len;
	
	va_start(arglist, _Format);
	fmt_len = wcslen(_Format) + 1;
	if( fmt_len == 0 )
		return;
	
	len = (size_t)_vscwprintf( _Format, arglist )+1;
	if( len < fmt_len ){
		len = fmt_len;
	}
	buffer = malloc( sizeof(wchar_t) * len );
	if( buffer ){
#if _MSC_VER >= 1400
		vswprintf_s ( buffer , len , _Format, arglist);
#else
		vswprintf( buffer , _Format, arglist);
#endif
	}
	if( buffer ){
#if defined(_CONSOLE)
		printf( buffer );
#else
		OutputDebugStringW(buffer);
#endif
	}
	if( buffer ){free(buffer); buffer = NULL; }
	va_end(arglist);
#endif
}
