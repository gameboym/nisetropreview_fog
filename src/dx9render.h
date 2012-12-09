
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
	@brief	Direct3D初期化用のパラメータを取得する
	@param	pD3DPP	[out]格納するためのポインタ
	@param	hWnd	[in]ウィンドウハンドル
	@param	w		[in]画面の幅
	@param	h		[in]画面の高さ
*/
void D3DGetPParameters( D3DPRESENT_PARAMETERS* pD3DPP , HWND hWnd , long w , long h );

/*!
	@brief	Direct3Dの初期化
	@param	pD3D	[out]格納するためのポインタ
	@param	pD3DDev	[out]格納するためのポインタ
	@param	hWnd	[in]ウィンドウハンドル
	@param	width	[in]画面の幅
	@param	height	[in]画面の高さ
*/
int D3DInit( LPDIRECT3D9* pD3D , LPDIRECT3DDEVICE9* pD3DDev , HWND hWnd , long width , long height );

/*!
	@brief	Direct3Dの終了処理
	@param	pD3D	[out]格納するためのポインタ
	@param	pD3DDev	[out]格納するためのポインタ
*/
void D3DQuit( LPDIRECT3D9* pD3D , LPDIRECT3DDEVICE9* pD3DDev );

/*!
	@brief	設定情報に基づいてウィンドウサイズを変更する。
	@param	pAppConfig	[in] -
	@param	hwnd		[in] -
*/
void D3DUpdateScreenConfig( LPAPPCONFIG pAppConfig , HWND hwnd );

#endif

