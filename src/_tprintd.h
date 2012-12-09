
#include <tchar.h>

#ifndef __TPRINTD_H__
#define __TPRINTD_H__

/*!
	@brief	�G���[�\���p�֐�(���C�h�o�C�g������p)
	@note	�g������_tprintf�Ƃقڈꏏ
*/
void wprintd( const wchar_t * _Format, ... );

/*!
	@brief	�G���[�\���p�֐�(�}���`�o�C�g������p)
	@note	�g������_tprintf�Ƃقڈꏏ
*/
void printd( const char * _Format, ... );

#if defined(_UNICODE)
#define _tprintd	wprintd
#else
#define _tprintd	printd
#endif

#endif
