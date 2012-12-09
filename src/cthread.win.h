// ���{��
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <process.h>

#ifndef __CTHREAD_WIN_H__
#define __CTHREAD_WIN_H__

/*!
 * @brief	�X���b�h�p�N���X(Windows�p)
 * */
class CTHREAD_WIN
{
private:
	//! �ҋ@���ԁims�j
	int m_iWait;
	
	//! �n���h��
	HANDLE m_hThread;
	
	//! ���[�v�p�t���O
	bool m_bWork;

	//! �X���b�h���[�v�p�֐�
	static unsigned int __stdcall thread_func( void* pData );
protected:

	//! �p���p�֐�
	virtual unsigned int ThreadFunction( void ) = 0;

public:

	//! �R���X�g���N�^
	CTHREAD_WIN();
	//! �f�X�g���N�^
	~CTHREAD_WIN();

	//! �X���b�h�N��
	void startThread( void );

	//! �X���b�h��~
	void stopThread( void );

	//! ���݂̏�Ԃ��擾
	bool IsWork( void ){return m_bWork;}
	
	//! �X���b�h���[�v�̑ҋ@���Ԃ��擾(ms)
	int getSleepTime( void ){ return m_iWait; }
	
	//! �X���b�h���[�v�̑ҋ@���Ԃ�ݒ�(ms)
	int setSleepTime( int iWait ){ if(iWait > 0 )m_iWait = iWait;  return m_iWait; }
};


#endif


