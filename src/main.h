#include "nisetro.h"
#include "configio.h"

#ifndef __MAIN_H__
#define __MAIN_H__

/*!
	@brief	Direct3D�̃��\�[�X���m�ۂ���
	@param	pD3DDev	[in]Direct3D�f�o�C�X
*/
int D3DResInit( LPDIRECT3DDEVICE9* pD3DDev );

/*!
	@brief	Direct3D�̃��\�[�X���J������
*/
void D3DResQuit( void );

/*!
	@brief	FPS���擾����
	@return FPS�� double �ŕԂ����
	@note	�����t���[���Ăяo���K�v������
*/
double GetFPS( void );


//! �`��֐�
void ReanderScreen( void );

//! �`��E�B���h�E�p�̃R�[���o�b�N
LRESULT CALLBACK WindowProc(HWND hwnd ,UINT uMsg,WPARAM wParam,LPARAM lParam);

HWND InitWindow( HINSTANCE hInst , WNDPROC CallBack );

/*!
	@fn void AppQuit( void )
	@brief �A�v���P�[�V�����̏I������
*/
void AppQuit( void );

//���C��
int WINAPI _tWinMain( HINSTANCE hInst , HINSTANCE hPrevInstance , LPTSTR lpszCmdLine , int nCmdShow );

#endif

