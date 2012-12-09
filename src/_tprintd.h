
#include <tchar.h>

#ifndef __TPRINTD_H__
#define __TPRINTD_H__

/*!
	@brief	エラー表示用関数(ワイドバイト文字列用)
	@note	使い方は_tprintfとほぼ一緒
*/
void wprintd( const wchar_t * _Format, ... );

/*!
	@brief	エラー表示用関数(マルチバイト文字列用)
	@note	使い方は_tprintfとほぼ一緒
*/
void printd( const char * _Format, ... );

#if defined(_UNICODE)
#define _tprintd	wprintd
#else
#define _tprintd	printd
#endif

#endif
