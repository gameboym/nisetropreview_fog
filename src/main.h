#include "nisetro.h"
#include "configio.h"

#ifndef __MAIN_H__
#define __MAIN_H__

/*!
	@brief	Direct3Dのリソースを確保する
	@param	pD3DDev	[in]Direct3Dデバイス
*/
int D3DResInit( LPDIRECT3DDEVICE9* pD3DDev );

/*!
	@brief	Direct3Dのリソースを開放する
*/
void D3DResQuit( void );

/*!
	@brief	FPSを取得する
	@return FPSが double で返される
	@note	※毎フレーム呼び出す必要がある
*/
double GetFPS( void );


//! 描画関数
void ReanderScreen( void );

//! 描画ウィンドウ用のコールバック
LRESULT CALLBACK WindowProc(HWND hwnd ,UINT uMsg,WPARAM wParam,LPARAM lParam);

HWND InitWindow( HINSTANCE hInst , WNDPROC CallBack );

/*!
	@fn void AppQuit( void )
	@brief アプリケーションの終了処理
*/
void AppQuit( void );

//メイン
int WINAPI _tWinMain( HINSTANCE hInst , HINSTANCE hPrevInstance , LPTSTR lpszCmdLine , int nCmdShow );

#endif

