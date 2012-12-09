
#include <windows.h>

#include <d3dx9.h>
#include <dxerr.h>
#include "configio.h"


extern "C" {
#include "_tprintd.h"
};

#ifndef __DX9RENDER_H__
#define __DX9RENDER_H__

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(x) { if(x){(x)->Release(); (x) = NULL;} }
#endif

/*!
	@brief	Direct3D�������p�̃p�����[�^���擾����
	@param	pD3DPP	[out]�i�[���邽�߂̃|�C���^
	@param	hWnd	[in]�E�B���h�E�n���h��
	@param	w		[in]��ʂ̕�
	@param	h		[in]��ʂ̍���
*/
void D3DGetPParameters( D3DPRESENT_PARAMETERS* pD3DPP , HWND hWnd , long w , long h );

/*!
	@brief	Direct3D�̏�����
	@param	pD3D	[out]�i�[���邽�߂̃|�C���^
	@param	pD3DDev	[out]�i�[���邽�߂̃|�C���^
	@param	hWnd	[in]�E�B���h�E�n���h��
	@param	width	[in]��ʂ̕�
	@param	height	[in]��ʂ̍���
*/
int D3DInit( LPDIRECT3D9* pD3D , LPDIRECT3DDEVICE9* pD3DDev , HWND hWnd , long width , long height );

/*!
	@brief	Direct3D�̏I������
	@param	pD3D	[out]�i�[���邽�߂̃|�C���^
	@param	pD3DDev	[out]�i�[���邽�߂̃|�C���^
*/
void D3DQuit( LPDIRECT3D9* pD3D , LPDIRECT3DDEVICE9* pD3DDev );

/*!
	@brief	�ݒ���Ɋ�Â��ăE�B���h�E�T�C�Y��ύX����B
	@param	pAppConfig	[in] -
	@param	hwnd		[in] -
*/
void D3DUpdateScreenConfig( LPAPPCONFIG pAppConfig , HWND hwnd );

#endif

