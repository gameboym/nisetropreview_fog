
#include "dx9render.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"dxerr9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9.lib")

/*!
	@brief	Direct3D初期化用のパラメータを取得する
	@param	pD3DPP	[out]格納するためのポインタ
	@param	hWnd	[in]ウィンドウハンドル
	@param	w		[in]画面の幅
	@param	h		[in]画面の高さ
*/
void D3DGetPParameters( D3DPRESENT_PARAMETERS* pD3DPP , HWND hWnd , long w , long h ){
	memset( pD3DPP , 0 , sizeof(D3DPRESENT_PARAMETERS) );
	pD3DPP->Windowed			= TRUE;
	pD3DPP->SwapEffect			= D3DSWAPEFFECT_DISCARD;
	pD3DPP->BackBufferCount		= 1;
	pD3DPP->BackBufferWidth		= w;
	pD3DPP->BackBufferHeight	= h;
	pD3DPP->BackBufferFormat	= D3DFMT_X8R8G8B8;
	pD3DPP->hDeviceWindow		= hWnd;
	pD3DPP->PresentationInterval= D3DPRESENT_INTERVAL_IMMEDIATE;
}

/*!
	@brief	Direct3Dの初期化
	@param	pD3D	[out]格納するためのポインタ
	@param	pD3DDev	[out]格納するためのポインタ
	@param	hWnd	[in]ウィンドウハンドル
	@param	width	[in]画面の幅
	@param	height	[in]画面の高さ
*/
int D3DInit( LPDIRECT3D9* pD3D , LPDIRECT3DDEVICE9* pD3DDev , HWND hWnd , long width , long height ){
	HRESULT hr;
	
	_tprintd( TEXT("D3DInit start\n") );
	(*pD3D) = Direct3DCreate9( D3D_SDK_VERSION );
	if( (*pD3D) == NULL ){
		_tprintd( TEXT("Direct3DCreate9 Error\n") );
		return -1;
	}
	
	_tprintd( TEXT("D3DGetPParameters\n") );
	D3DPRESENT_PARAMETERS d3dpp;
	D3DGetPParameters( &d3dpp , hWnd , width , height );
	
	_tprintd( TEXT("CreateDevice\n") );
	hr = (*pD3D)->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd , D3DCREATE_HARDWARE_VERTEXPROCESSING , &d3dpp , pD3DDev );
	if( hr ){
		hr = (*pD3D)->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , hWnd , D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , pD3DDev );
		if( hr ){
			hr = (*pD3D)->CreateDevice( D3DADAPTER_DEFAULT , D3DDEVTYPE_REF , hWnd , D3DCREATE_SOFTWARE_VERTEXPROCESSING , &d3dpp , pD3DDev );
			if( hr ){
				_tprintd( TEXT("CreateDevice Error\n") );
				return -1;
			}
		}
	}
	
	_tprintd( TEXT("D3DInit end\n") );
	return 0;
}

/*!
	@brief	Direct3Dの終了処理
	@param	pD3D	[out]格納するためのポインタ
	@param	pD3DDev	[out]格納するためのポインタ
*/
void D3DQuit( LPDIRECT3D9* pD3D , LPDIRECT3DDEVICE9* pD3DDev ){
	SAFE_RELEASE( (*pD3DDev) );
	SAFE_RELEASE( (*pD3D) );
}



void D3DUpdateScreenConfig( LPAPPCONFIG pAppConfig , HWND hwnd ){
	if( pAppConfig == NULL ) return;
	if( hwnd == NULL ) return;
	_tprintd( TEXT("UpdateScreenConfig start\n") );
	RECT rcW , rcC;
	GetWindowRect( hwnd , &rcW );
	GetClientRect( hwnd , &rcC );
	
	long Ww , Wh , Cw , Ch;
	Ww = rcW.right - rcW.left;	Wh = rcW.bottom - rcW.top;
	Cw = rcC.right - rcC.left;	Ch = rcC.bottom - rcC.top;
	_tprintd( TEXT("before window size %d %d\n") , Ww , Wh );
	_tprintd( TEXT("before client size %d %d\n") , Cw , Ch );
	
	// 装飾の部分のサイズを取得する
	Ww = (Ww - Cw);
	Wh = (Wh - Ch);
	_tprintd( TEXT("     sousyoku size %d %d\n") , Ww , Wh );
	
	RECT rc;
	GetClientSizeForAppconfig( pAppConfig , &rc );
	
	Ww += rc.right;
	Wh += rc.bottom;
	
	_tprintd( TEXT("after  client size %d %d\n") , rc.right , rc.bottom );
	MoveWindow( hwnd , rcW.left , rcW.top , Ww , Wh , TRUE );
	_tprintd( TEXT("UpdateScreenConfig end\n") );
}

