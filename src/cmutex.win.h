
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <windows.h>

#ifndef CMUTEX_WIN_H_
#define CMUTEX_WIN_H_

/*!
 * @brief	�~���[�e�N�X�p�N���X(Windows�p)
 * */
class CMUTEX_WIN
{
protected:
	//! �n���h��
	HANDLE m_hMutex;
public:
	//! �R���X�g���N�^
	CMUTEX_WIN();
	//! �f�X�g���N�^
	~CMUTEX_WIN();

	//! �~���[�e�N�X�̍쐬
	void create( void );

	//! �~���[�e�N�X�̉��
	void destroy( void );

	//! ���b�N�̎擾
	int lock( DWORD dwMSec );

	//! ���b�N�̉��
	int unlock( void );
};



#endif /* CMUTEX_WIN_H_ */
