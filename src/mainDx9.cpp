
/*!
	@file
*/

#include <windows.h>
#include <commctrl.h>
#include <locale.h>

#include <stdio.h>
#include <math.h>
#include <tchar.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <time.h>
#include <string>
#include "dx9render.h"

#include "cthread.h"
#include "cmutex.h"

#include "nisetro.h"
#include "gameconsole.h"
#include "main.h"

#include "_tprintd.h"

#include "configio.h"
#include "blur.h"

#include "res\resource.h"

#if defined(_MSC_VER)
	#if _MSC_VER >= 1500
	#include "res\res_msvc9.h"
	#else
	#include "res\res_msvc8.h"
	#endif
#endif

#pragma comment(lib,"winmm.lib")

CCONSOLEGAMES	g_console(CONSOLE_GAMEBOYCOLOR);	// CNISETRO�̑O�ɐ�������K�v�L
CNISETRO		g_nise(g_console.GetScreenSize(), g_console.GetCCapBufSize());
TEXTURE_BLUR	*g_pTexBlur;		// �c���Ή��e�N�X�`���Ǘ��N���X

CMUTEX		g_mutex_Texture;

HINSTANCE	g_hInst;
HWND		g_hWnd;					//�E�B���h�E�n���h��
HWND		g_hConfigDlg = NULL;	//�E�B���h�E�n���h��
HWND		g_hInfoDlg = NULL;		//�E�B���h�E�n���h��
HWND		g_hRecordDlg = NULL;	//�E�B���h�E�n���h��

//! app config
SAPPCONFIG g_AppConfig;

LPDIRECT3D9	g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pD3DDev = NULL;

LPD3DXSPRITE	g_pD3DXSprite = NULL;

D3DTEXTUREFILTERTYPE g_eFilter = D3DTEXF_NONE;
D3DTEXTUREFILTERTYPE g_eFilterMin = D3DTEXF_NONE;

/*!
	@brief	Direct3D�̃��\�[�X���m�ۂ���
	@param	pD3DDev	[in]Direct3D�f�o�C�X
*/
int D3DResInit( LPDIRECT3DDEVICE9* pD3DDev ){
	HRESULT hr;
	g_pTexBlur = new TEXTURE_BLUR();

	if( g_pTexBlur->CreateTexture(&g_pD3DDev) ){
		_tprintd( TEXT("Error CreateTexture\n") );
		return -1;
	}

	hr = D3DXCreateSprite( (*pD3DDev) , &g_pD3DXSprite );
	if( hr ){
		_tprintd( TEXT("Error D3DXCreateSprite\n") );
		return -1;
	}
	
	return 0;
}

/*!
	@brief	Direct3D�̃��\�[�X���J������
*/
void D3DResQuit( void ){
	
	SAFE_RELEASE( g_pD3DXSprite );
	if (g_pTexBlur != NULL) {
		g_pTexBlur->ReleaseTexture();
		delete g_pTexBlur;
	}
}

//! xbit�̐F�f�[�^��8bit�ɕϊ�����
BYTE to8bitcolor(BYTE data, int bit){
	int i;
	BYTE mask = 0;

	if ( (bit > 8) || (bit < 1) ) return data;
	for (i = 0; i < bit; i++)
		mask = (mask << 1) | 1;

	data = data & mask;
	return (BYTE)(((float)data / (float)mask) * 255.0f);
}

//! �e�N�`���փo�b�t�@�̓��e����������
HRESULT BufToTex(const BYTE *p_ScrData, LPDIRECT3DTEXTURE9 pTex){
	D3DLOCKED_RECT LockRc;
	HRESULT hr;
	BYTE* pBits = NULL;

	if (pTex == NULL || p_ScrData == NULL) return -1;

	// Top�e�N�X�`���̃��b�N���擾
	hr = pTex->LockRect( 0 , &LockRc , NULL , 0 );
	if( hr ) return hr;

	pBits = (BYTE*)LockRc.pBits;
	for( int y = 0; y < g_console.GetHeight(); y++ ){
		pBits = (BYTE*)LockRc.pBits + LockRc.Pitch * y;
		for( int x = 0; x < g_console.GetWidth(); x++ ){
			for( int i = 2; i >= 0; i-- )
				(*pBits++) = to8bitcolor(p_ScrData[y*g_console.GetWidth()*3+x*3+i], g_console.GetCbit());
			(*pBits++) = 0xFF;
		}
		for( int j = 1; j >= 0; j--){	// 2�h�b�g���œh��Ԃ�(�t�B���^�g�p���S�~���c��̂�h��)
			for( int i = 2; i >= 0; i-- )
				(*pBits++) = 0x00;
			(*pBits++) = 0xFF;
		}
	}
	for( int x = 0; x < g_console.GetWidth()+2; x++ ){ // 2���C�����œh��Ԃ�(�t�B���^�g�p���S�~���c��̂�h��)
		for( int i = 2; i >= 0; i-- )
			(*pBits++) = 0x00;
		(*pBits++) = 0xFF;
	}

	pTex->UnlockRect(0);//���b�N�̉��
	return 0;
}

HRESULT ClrTex(LPDIRECT3DTEXTURE9 pTex){
	D3DLOCKED_RECT LockRc;
	HRESULT hr;
	BYTE* pBits = NULL;

	if (pTex == NULL) return -1;

	// Top�e�N�X�`���̃��b�N���擾
	hr = pTex->LockRect( 0 , &LockRc , NULL , 0 );
	if( hr ) return hr;

	pBits = (BYTE*)LockRc.pBits;
	for( int y = 0; y < TEXTURE_WIDTH; y++ ){
		pBits = (BYTE*)LockRc.pBits + LockRc.Pitch * y;
		for( int x = 0; x < TEXTURE_HEIGHT; x++ ){
			for( int i = 2; i >= 0; i-- )
				(*pBits++) = 0x00;
			(*pBits++) = 0xFF;
		}
	}

	pTex->UnlockRect(0);//���b�N�̉��
	return 0;
}

//! �o�b�t�@�ϊ��p�X���b�h
class CBUFtoTEX : public CTHREAD{
private:
	BYTE	*p_ScrData;
	
	void create_buffer(int scrsize){
		p_ScrData		= new BYTE[scrsize];
		memset( p_ScrData , 0xFF , sizeof(BYTE) * scrsize );
	}

	void delete_buffer(){
		if(p_ScrData != NULL) delete p_ScrData;
	}

public:
	CBUFtoTEX(){
		this->setSleepTime( 1 ); // �X���[�v�^�C���͍ŏ�
		create_buffer(g_console.GetScreenSize());
	}

	~CBUFtoTEX(){
		delete_buffer();
	}
	
	void setFps( long fps ){
		this->setSleepTime( 1 ); // �X���[�v�^�C���͍ŏ�
	}

	void change_scrsize(int scrsize) {
		if( g_mutex_Texture.lock(getSleepTime()) == false ){
			BYTE *swap = p_ScrData;
			create_buffer(scrsize);
			if(swap != NULL) delete swap;

			g_mutex_Texture.unlock();
		}
	}
	
	virtual unsigned int ThreadFunction( void ){
		int capcounter;

		if( !g_pTexBlur ) return -1;
		
		capcounter = g_nise.GetCapCounter();
		if( capcounter == 0 ) return -1;
		if( capcounter > 1 ) {
			static int c = 0;
			_tprintd( TEXT("%d %d ���擾�f�[�^�L CBUFtoTEX\n") , c++ , capcounter );
			g_nise.DecCapCounter();
		}
		g_nise.DecCapCounter();
		
		DWORD dwTime1 = timeGetTime();

		if( g_nise.GetScrData( p_ScrData , g_console.GetScreenSize() , getSleepTime() ) )
			return -1;

		g_pTexBlur->SetTexture(p_ScrData);

		DWORD dwTime2 = timeGetTime();
		DWORD dwWTime = this->getSleepTime() - (dwTime2 - dwTime1);
		if( dwWTime <= 0 ){
			static int c = 0;
			_tprintd( TEXT("%d %d ���������̉\�� CBUFtoTEX\n") , c++ , dwWTime );
			dwWTime = 0;
		}
		
		return 0;
	}
};

/*!
	@brief	FPS���擾����
	@return FPS�� double �ŕԂ����
	@note	�����t���[���Ăяo���K�v������
*/
double GetFPS( void )
{
	static DWORD	last = timeGetTime();
	static DWORD	frames = 0;
	static double	fps = 0;
	DWORD			current;
	
	current = timeGetTime();
	frames++;
	
	if(500 <= current - last) {// 0.5�b���ɍX�V
		double dt = (double)(current - last) / 1000.0f;
		fps = (double)frames / dt;
		last = current;
		frames = 0;
	}
	return fps;
}

/*!
	@brief �`��p�X���b�h
*/
class CRENDERTHREAD : public CTHREAD{
protected:
	DWORD m_dwMaxTime;
public:
	double m_dFPS;
	
	CRENDERTHREAD(){
		m_dFPS = 0;
		m_dwMaxTime = 1000 / g_console.GetFps();
		this->setSleepTime( m_dwMaxTime );
	}
	
	void setFps( long fps ){
		m_dwMaxTime = 1000 / fps;
		this->setSleepTime( m_dwMaxTime );
	}
	
	virtual unsigned int ThreadFunction( void ){
		DWORD dwTime1 = timeGetTime();
		if( g_mutex_Texture.lock(m_dwMaxTime) == false ){
			ReanderScreen();
			m_dFPS = GetFPS();
			g_mutex_Texture.unlock();
		}
		
		DWORD dwTime2 = timeGetTime();
		DWORD dwWTime = m_dwMaxTime - (dwTime2 - dwTime1);
		if( dwWTime <= 0 ){
			static int c = 0;
			_tprintd( TEXT("%d %d ���������̉\�� CRENDERTHREAD\n") , c++ , dwWTime );
			dwWTime = 0;
		}
		this->setSleepTime( dwWTime );
		return 0;
	}
};

//! �o�b�t�@����e�N�X�`���[�֕ϊ�����X���b�h
CBUFtoTEX		g_thBuftoTEX;
CRENDERTHREAD	g_thRender;

//! �`��֐�
void ReanderScreen( void )
{
	static 
	PAPPCONFIG pAppConfig = &g_AppConfig;
	g_pD3DDev->BeginScene();
	
	//�����ŕ`��
	if( g_pTexBlur && g_pD3DXSprite ){
		RECT SrcRc = { 0 , 0 , g_console.GetWidth() , g_console.GetHeight() };
		D3DXVECTOR3 vecCenter( 0.0f , 0.0f ,0.0f );
		D3DXVECTOR3 vecPos( 0.0f , 0.0f ,0.0f );
		D3DXMATRIX mat;
		D3DXMATRIX matRot;
		D3DXMATRIX matTrans;
		D3DXMATRIX matScal;
		D3DXMatrixScaling( &matScal , pAppConfig->m_fScrScal , pAppConfig->m_fScrScal , 0.0f );
		float rot = 0.0f;
		float rx = 0.0f, ry = 0.0f;
		float x = 0.0f , y = 0.0f;
			
		if( pAppConfig->m_DirMode == DIRMODE_NORMAL ){
			rot	= 0.0f;
			rx	= 0;
			ry	= 0;
			x = 0;
			y = 0;
		}else if( pAppConfig->m_DirMode == DIRMODE_LEFT ){
			rot	= -90.0f;
			rx	= 0;
			ry	= (float)g_console.GetWidth();
			x = 0;
			y = 0;
		}else if( pAppConfig->m_DirMode == DIRMODE_RIGHT ){
			rot	= 90.0f;
			rx	= (float)g_console.GetHeight();
			ry	= 0;
			x = 0;
			y = 0;
		}else if( pAppConfig->m_DirMode == DIRMODE_KERORICAN ){
			rot	= DEGREE_KERORICAN;
			rx	= 0;
			ry	= g_console.GetWidth() * fabs( sin(D3DXToRadian(rot)) );
			x = 0;
			y = 0;
		}
		{
			D3DXMatrixRotationZ( &matRot, D3DXToRadian( rot ) );
			matRot._41 = rx;   // X��
			matRot._42 = ry;    // Y��
			D3DXMatrixTranslation( &matTrans, ((float)x) , ((float)y) , 0.0f );
			D3DXMatrixMultiply( &mat , &matRot , &matTrans );
			D3DXMatrixMultiply( &mat , &mat , &matScal );
			g_pD3DXSprite->SetTransform( &mat );
		}
		g_pD3DXSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_DONOTSAVESTATE );

		g_pD3DDev->SetSamplerState( 0 , D3DSAMP_ADDRESSU , D3DTADDRESS_BORDER );
		g_pD3DDev->SetSamplerState( 0 , D3DSAMP_ADDRESSV , D3DTADDRESS_BORDER );
		g_pD3DDev->SetSamplerState( 0 , D3DSAMP_ADDRESSW , D3DTADDRESS_BORDER );

		if(g_eFilter == D3DTEXF_NONE)	// AA�؂��������ǂ��H
			g_pD3DDev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , FALSE );
		else
			g_pD3DDev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , TRUE );

		g_pD3DDev->SetSamplerState( 0 , D3DSAMP_MAGFILTER , g_eFilter );
		g_pD3DDev->SetSamplerState( 0 , D3DSAMP_MINFILTER , g_eFilter );
		g_pD3DDev->SetSamplerState( 0 , D3DSAMP_MIPFILTER , g_eFilter );
	
		g_pTexBlur->RenderBlur(&SrcRc, &vecCenter, &vecPos, 0);
		g_pD3DXSprite->End();
	}

	g_pD3DDev->EndScene();
	g_pD3DDev->Present( NULL , NULL , NULL , NULL );
}

#include "dlgproc_config.h"
#include "dlgproc_info.h"

void change_console(){
	g_nise.change_buffersize(g_console.GetScreenSize(g_AppConfig.m_iconsole), g_console.GetCCapBufSize(g_AppConfig.m_iconsole));
	g_thBuftoTEX.change_scrsize(g_console.GetScreenSize(g_AppConfig.m_iconsole));
	g_thRender.setFps( g_console.GetFps(g_AppConfig.m_eFrmSkip, g_AppConfig.m_iconsole) );
	g_thBuftoTEX.setFps( g_console.GetFps(g_AppConfig.m_eFrmSkip, g_AppConfig.m_iconsole) );
	g_console.SetConsole(g_AppConfig.m_iconsole);
}

//! �`��E�B���h�E�p�̃R�[���o�b�N
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	
	switch( uMsg )
	{
		case WM_CREATE:
			{
				HMENU hMenu = GetMenu( hwnd );
				if( hMenu ){
					CheckMenuItem( hMenu , ID_MENU_TOPWINDOW , (g_AppConfig.m_bTopWindow?MF_CHECKED:MF_UNCHECKED) );
					CheckMenuItem( hMenu , ID_MENU_DROP_ERRFRAME , (g_AppConfig.m_bDropFrame?MF_CHECKED:MF_UNCHECKED) );
				}
				if( g_AppConfig.m_bTopWindow ){
					g_AppConfig.m_bTopWindow = false;
					SendMessage( hwnd , WM_COMMAND , MAKEWPARAM(ID_MENU_TOPWINDOW,0) , 0 );
				}
				SetTimer( hwnd , 1421356 , (1000 / 2) , NULL );
				//
			}
			break;
		
		case WM_DESTROY:
			{
				KillTimer( hwnd, 1421356 );
			}
			break;
		
		case WM_TIMER:
			{
				TCHAR strBuf[256];
				TCHAR strTitle[256];
				LoadString( (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE) , IDS_STRING_TITLE , strTitle , 256 );
#if _MSC_VER >= 1400
				_stprintf_s( strBuf , 256 , TEXT("%s ID:%d") , strTitle , g_AppConfig.m_iCUSB2_ID );
#else
				_stprintf( strBuf , TEXT("%s ID:%d") , strTitle , g_AppConfig.m_iCUSB2_ID );
#endif
				SetWindowText( g_hWnd , strBuf );
			}
			break;
		
		case WM_CLOSE:
				if( g_hRecordDlg ){
					DestroyWindow( g_hRecordDlg );
					g_hRecordDlg = NULL;
				}
				PostQuitMessage(0);
			break;
		
		// �f�o�C�X���X�g��
		case WM_DEVICECHANGE:
			{
				HMENU hMenu;
				_tprintd( TEXT("WM_DEVICECHANGE\n") );
				
				
				PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
				lpdb = lpdb;
				if( wParam == DBT_CONFIGCHANGECANCELED )	_tprintd( TEXT(" DBT_CONFIGCHANGECANCELED\n") );
				if( wParam == DBT_CONFIGCHANGED )			_tprintd( TEXT(" DBT_CONFIGCHANGED\n") );
				if( wParam == DBT_CUSTOMEVENT )				_tprintd( TEXT(" DBT_CUSTOMEVENT\n") );
				if( wParam == DBT_DEVICEARRIVAL )			_tprintd( TEXT(" DBT_DEVICEARRIVAL\n") );
				if( wParam == DBT_DEVICEQUERYREMOVE )		_tprintd( TEXT(" DBT_DEVICEQUERYREMOVE\n") );
				if( wParam == DBT_DEVICEQUERYREMOVEFAILED )	_tprintd( TEXT(" DBT_DEVICEQUERYREMOVEFAILED\n") );
				if( wParam == DBT_DEVICEREMOVECOMPLETE )	_tprintd( TEXT(" DBT_DEVICEREMOVECOMPLETE\n") );
				if( wParam == DBT_DEVICEREMOVEPENDING )		_tprintd( TEXT(" DBT_DEVICEREMOVEPENDING\n") );
				if( wParam == DBT_DEVICETYPESPECIFIC )		_tprintd( TEXT(" DBT_DEVICETYPESPECIFIC\n") );
				if( wParam == DBT_DEVNODES_CHANGED )		_tprintd( TEXT(" DBT_DEVNODES_CHANGED\n") );
				if( wParam == DBT_QUERYCHANGECONFIG )		_tprintd( TEXT(" DBT_QUERYCHANGECONFIG\n") );
				if( wParam == DBT_USERDEFINED )				_tprintd( TEXT(" DBT_USERDEFINED\n") );
				
				cusb2* pUsb = g_nise.GetCUSB2();
				if( g_nise.IsInit() ){
					if( pUsb )
					if( pUsb->PnpEvent( wParam , lParam) ){
						_tprintd( TEXT("PnpEvent\n") );
						
						
						hMenu = GetMenu( hwnd );
						for( int i = 0; i < 4; i++ ){
							if( hMenu )EnableMenuItem( hMenu , (ID_MENU_CUSB2_ID0+i) , MF_GRAYED );
						}
						g_nise.NisetroQuit();
						if( g_nise.NisetroInit( g_AppConfig.m_eFrmSkip , g_AppConfig.m_iCUSB2_ID , hwnd ) ){
							// error
							g_nise.NisetroQuit();
						}
						
						hMenu = GetMenu( hwnd );
						for( int i = 0; i < 4; i++ ){
							if( hMenu )EnableMenuItem( hMenu , (ID_MENU_CUSB2_ID0+i) , MF_ENABLED );
						}
						
					}
				}else{
						g_nise.NisetroQuit();
						if( g_nise.NisetroInit( g_AppConfig.m_eFrmSkip , g_AppConfig.m_iCUSB2_ID , hwnd ) ){
							g_nise.NisetroQuit();
						}
						
						hMenu = GetMenu( hwnd );
						for( int i = 0; i < 4; i++ ){
							if( hMenu )EnableMenuItem( hMenu , (ID_MENU_CUSB2_ID0+i) , MF_ENABLED );
						}
						
				}
			}
			break;
		
		case WM_COMMAND:
			{
				int id = LOWORD(wParam);
				switch (id) {
					case ID_MENU_REC:
						break;
					
					case ID_MENU_SETTING:
						{
							if( g_hConfigDlg )break;
							g_hConfigDlg = CreateDialog( g_hInst , MAKEINTRESOURCE(IDD_DLG_CONFIG) , hwnd , (DLGPROC)ConfigDlgProc );
							ShowWindow(g_hConfigDlg, SW_SHOW);
							UpdateWindow(g_hConfigDlg); 
						}
						break;
					
					case ID_MENU_INFO:
						{
							if( g_hInfoDlg )break;
							g_hInfoDlg = CreateDialog( g_hInst , MAKEINTRESOURCE(IDD_DLG_INFO) , hwnd , (DLGPROC)InfoDlgProc );
							ShowWindow(g_hInfoDlg, SW_SHOW);
							UpdateWindow(g_hInfoDlg); 
						}
						break;
					
					// ��Ɏ�O�ɕ\������
					case ID_MENU_TOPWINDOW:
						{
							RECT rc;
							GetWindowRect( hwnd , &rc );
							if( g_AppConfig.m_bTopWindow ){
								SetWindowPos( hwnd , HWND_NOTOPMOST , rc.left , rc.top , rc.right - rc.left , rc.bottom - rc.top , 0 );
							}else{
								SetWindowPos( hwnd , HWND_TOPMOST , rc.left , rc.top , rc.right - rc.left , rc.bottom - rc.top , 0 );
							}
							g_AppConfig.m_bTopWindow = (g_AppConfig.m_bTopWindow ? false : true);
							HMENU hMenu = GetMenu( hwnd );
							if( hMenu )
								CheckMenuItem( hMenu , ID_MENU_TOPWINDOW , (g_AppConfig.m_bTopWindow?MF_CHECKED:MF_UNCHECKED) );
						}
						break;
					
					// �G���[�t���[����\�����Ȃ�
					case ID_MENU_DROP_ERRFRAME:
						{
							g_AppConfig.m_bDropFrame = (g_AppConfig.m_bDropFrame ? false : true);
							g_nise.SetDropFrame( g_AppConfig.m_bDropFrame );
							HMENU hMenu = GetMenu( hwnd );
							if( hMenu )
								CheckMenuItem( hMenu , ID_MENU_DROP_ERRFRAME , (g_AppConfig.m_bDropFrame?MF_CHECKED:MF_UNCHECKED) );
						}
						break;
					
					case ID_MENU_END:
						SendMessage( hwnd , WM_CLOSE , 0 , 0 );
						break;
					
					// ��ʂ̌���
					case ID_MENU_SRCROT_NORMAL:
					case ID_MENU_SRCROT_LEFT:
					case ID_MENU_SRCROT_RIGHT:
					case ID_MENU_SRCROT_KERORICAN:
						{
							if( id == ID_MENU_SRCROT_NORMAL )		g_AppConfig.m_DirMode = DIRMODE_NORMAL;
							if( id == ID_MENU_SRCROT_LEFT )			g_AppConfig.m_DirMode = DIRMODE_LEFT;
							if( id == ID_MENU_SRCROT_RIGHT )		g_AppConfig.m_DirMode = DIRMODE_RIGHT;
							if( id == ID_MENU_SRCROT_KERORICAN )	g_AppConfig.m_DirMode = DIRMODE_KERORICAN;
							D3DUpdateScreenConfig( &g_AppConfig , g_hWnd );
							if( g_hConfigDlg )SendMessage( GetDlgItem(g_hConfigDlg, IDC_COMBO_SCRDIR) , CB_SETCURSEL , g_AppConfig.m_DirMode , 0 );
						}
						break;
					
					// �{��
					case ID_MENU_SRCSCAL_X1:
					case ID_MENU_SRCSCAL_X2:
					case ID_MENU_SRCSCAL_X3:
					case ID_MENU_SRCSCAL_X4:
					case ID_MENU_SRCSCAL_X5:
						{
							if( id == ID_MENU_SRCSCAL_X1 )	g_AppConfig.m_fScrScal = 1.0f;
							if( id == ID_MENU_SRCSCAL_X2 )	g_AppConfig.m_fScrScal = 2.0f;
							if( id == ID_MENU_SRCSCAL_X3 )	g_AppConfig.m_fScrScal = 3.0f;
							if( id == ID_MENU_SRCSCAL_X4 )	g_AppConfig.m_fScrScal = 4.0f;
							if( id == ID_MENU_SRCSCAL_X5 )	g_AppConfig.m_fScrScal = 5.0f;
							D3DUpdateScreenConfig( &g_AppConfig , g_hWnd );
							if( g_hConfigDlg ){
								SendMessage( GetDlgItem(g_hConfigDlg, IDC_SLIDER_SCRSCALE) , TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(g_AppConfig.m_fScrScal*100));
								SendMessage( g_hConfigDlg , WM_HSCROLL, (WPARAM)TRUE, (LPARAM)GetDlgItem(g_hConfigDlg, IDC_SLIDER_SCRSCALE));
							}
						}
						break;

					// �t���[����
					case ID_MENU_60FPS:
					case ID_MENU_30FPS:
					case ID_MENU_20FPS:
					case ID_MENU_15FPS:
						{
							long fps = 0;
							if( id == ID_MENU_60FPS ){	g_AppConfig.m_eFrmSkip = ECAPFPS_60; fps = g_console.GetFps(ECAPFPS_60);}
							if( id == ID_MENU_30FPS ){	g_AppConfig.m_eFrmSkip = ECAPFPS_30; fps = g_console.GetFps(ECAPFPS_30);}
							if( id == ID_MENU_20FPS ){	g_AppConfig.m_eFrmSkip = ECAPFPS_20; fps = g_console.GetFps(ECAPFPS_20);}
							if( id == ID_MENU_15FPS ){	g_AppConfig.m_eFrmSkip = ECAPFPS_15; fps = g_console.GetFps(ECAPFPS_15);}
							if( g_hConfigDlg ){
								WPARAM send = (id-ID_MENU_60FPS);
								SendMessage( GetDlgItem(g_hConfigDlg, IDC_COMBO_FPS) , CB_SETCURSEL , send , 0 );
							}
							g_thRender.setFps( fps );
							g_thBuftoTEX.setFps( fps );
							g_nise.NisetroQuit();
							g_nise.NisetroInit( g_AppConfig.m_eFrmSkip , g_AppConfig.m_iCUSB2_ID , g_hWnd );
						}
						break;						
					case ID_MENU_CUSB2_ID0:
					case ID_MENU_CUSB2_ID1:
					case ID_MENU_CUSB2_ID2:
					case ID_MENU_CUSB2_ID3:
					case ID_MENU_CUSB2_ID4:
					case ID_MENU_CUSB2_ID5:
					case ID_MENU_CUSB2_ID6:
					case ID_MENU_CUSB2_ID7:
					case ID_MENU_CUSB2_ID8:
					case ID_MENU_CUSB2_ID9:
						{
							int cusb2id = id - ID_MENU_CUSB2_ID0;
							if( cusb2id >= 0 && cusb2id <= 9 ){
								g_AppConfig.m_iCUSB2_ID = cusb2id;
								g_nise.NisetroQuit();
								g_nise.NisetroInit( g_AppConfig.m_eFrmSkip , g_AppConfig.m_iCUSB2_ID , hwnd );
							}
						}
						break;
						
					case ID_MENU_D3DFILTER_NONE:
					case ID_MENU_D3DFILTER_POINT:
					case ID_MENU_D3DFILTER_LINEAR:
					case ID_MENU_D3DFILTER_ANISOTROPIC:
					case ID_MENU_D3DFILTER_PYRAMIDALQUAD:
					case ID_MENU_D3DFILTER_GAUSSIANQUAD:
						{
							if( id == ID_MENU_D3DFILTER_NONE )			g_eFilter = D3DTEXF_NONE;
							if( id == ID_MENU_D3DFILTER_POINT )			g_eFilter = D3DTEXF_POINT;
							if( id == ID_MENU_D3DFILTER_LINEAR )		g_eFilter = D3DTEXF_LINEAR;
							if( id == ID_MENU_D3DFILTER_ANISOTROPIC )	g_eFilter = D3DTEXF_ANISOTROPIC;
							if( id == ID_MENU_D3DFILTER_PYRAMIDALQUAD )	g_eFilter = D3DTEXF_PYRAMIDALQUAD;
							if( id == ID_MENU_D3DFILTER_GAUSSIANQUAD )	g_eFilter = D3DTEXF_GAUSSIANQUAD;
						}
						break;
					case ID_MENU_BLUR_NONE:
					case ID_MENU_BLUR_GBCMODE:
					case ID_MENU_BLUR_GBMODE:
					case ID_MENU_BLUR_GBPMODE:
					case ID_MENU_BLUR_WSCMODE:
						{
							if( id == ID_MENU_BLUR_NONE )		g_pTexBlur->SetBlurType(BLUR_TYPE_NONE);
							if( id == ID_MENU_BLUR_GBCMODE )	g_pTexBlur->SetBlurType(BLUR_TYPE_GBCMODE);
							if( id == ID_MENU_BLUR_GBMODE )		g_pTexBlur->SetBlurType(BLUR_TYPE_GBMODE);
							if( id == ID_MENU_BLUR_GBPMODE )	g_pTexBlur->SetBlurType(BLUR_TYPE_GBPMODE);
							if( id == ID_MENU_BLUR_WSCMODE )	g_pTexBlur->SetBlurType(BLUR_TYPE_WSCMODE);
						}
						break;

					case ID_MENU_CONSOLE_GBC:
					case ID_MENU_CONSOLE_OGB:
					case ID_MENU_CONSOLE_NGPC:
					case ID_MENU_CONSOLE_SC:
					case ID_MENU_CONSOLE_PIECE:
						{
							g_nise.NisetroQuit();
							if( id == ID_MENU_CONSOLE_GBC )		g_AppConfig.m_iconsole = CONSOLE_GAMEBOYCOLOR;
							if( id == ID_MENU_CONSOLE_OGB )		g_AppConfig.m_iconsole = CONSOLE_ORIGINALGAMEBOY;
							if( id == ID_MENU_CONSOLE_NGPC )	g_AppConfig.m_iconsole = CONSOLE_NEOGEOPOCKETCOLOR;
							if( id == ID_MENU_CONSOLE_SC )		g_AppConfig.m_iconsole = CONSOLE_SWANCRYSTAL;
							if( id == ID_MENU_CONSOLE_PIECE )	g_AppConfig.m_iconsole = CONSOLE_PIECE;
							change_console();
							g_nise.NisetroInit( g_AppConfig.m_eFrmSkip, g_AppConfig.m_iCUSB2_ID, hwnd);
							D3DUpdateScreenConfig( &g_AppConfig , g_hWnd );
						}
						break;
						
					// �X�N���[���V���b�g
					case ID_MENU_SCREENSHOT:
						{
							// �ǂ̃^�C�~���O�Ń��b�N���邩�E�E�E
							D3DSURFACE_DESC desc;
							LPDIRECT3DTEXTURE9 pTex = NULL;
							LPDIRECT3DSURFACE9 pSurSave = NULL;
							TCHAR strFileName[MAX_PATH];
							TCHAR strFileTitle[MAX_PATH];
							memset( strFileName , 0 , sizeof(TCHAR) * MAX_PATH );
							memset( strFileTitle , 0 , sizeof(TCHAR) * MAX_PATH );
							
							
							OPENFILENAME ofn;
							memset( &ofn , 0 , sizeof(OPENFILENAME) );
							
							ofn.lStructSize		= sizeof(OPENFILENAME); 
							ofn.hInstance		= (HINSTANCE)GetWindowLongPtr( hwnd , GWLP_HINSTANCE );
							ofn.hwndOwner		= hwnd;
							ofn.lpstrFilter		= TEXT("pngfile(*.png)\0*.png\0\01");
							ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
							ofn.lpstrDefExt		= TEXT("png");
							ofn.lpstrFileTitle	= strFileTitle;
							ofn.nMaxFileTitle	= MAX_PATH;
							ofn.lpstrFile		= strFileName;
							ofn.nMaxFile		= MAX_PATH;
							
							// �ۑ�����T�[�t�F�C�X���m�ۂ���
							if( g_mutex_Texture.lock(INFINITE) == false ){
								if( g_pD3DDev ){
									LPDIRECT3DSURFACE9 pSur = NULL;
									g_pD3DDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO , &pSur );
									pSur->GetDesc(&desc);
									
									// �e�N�X�`���̃T�C�Y�����߂�(2�̔{���łȂ���΂Ȃ�Ȃ�)
									UINT w , h;
									for( w = 1; w <= desc.Width; w*= 2 );
									for( h = 1; h <= desc.Height; h*= 2 );
									
									_tprintd( TEXT("tex %d , %d\n") , w , h );
									g_pD3DDev->CreateTexture( w , h , 1 , D3DUSAGE_DYNAMIC , D3DFMT_A8R8G8B8 , D3DPOOL_DEFAULT , &pTex , NULL );
									if( pTex )pTex->GetSurfaceLevel( 0 , &pSurSave ); else{ MessageBox(NULL , TEXT("e") , TEXT("error texture") , MB_OK ); }
									// ���̂܂܂ł̓t�@�C�������w�蒆�ɃT�[�t�F�C�X�̓��e���ς���Ă��܂��̂ŃR�s�[����
									if( pSur && pSurSave ){
										D3DXLoadSurfaceFromSurface( pSurSave , NULL , NULL , pSur , NULL , NULL , D3DX_FILTER_NONE , 0 );
									} else{ MessageBox(NULL , TEXT("e") , TEXT("error surface copy") , MB_OK ); }
									SAFE_RELEASE( pSur );
								}
								g_mutex_Texture.unlock();
							}
							
							// �ۑ�����ꏊ��₢������
							if( GetSaveFileName( &ofn ) ){
								//	break point
								strFileTitle[MAX_PATH - 1]	= NULL;
								strFileName[MAX_PATH - 1]	= NULL;
								ofn.lpstrFileTitle			= ofn.lpstrFileTitle;
								ofn.lpstrFile				= ofn.lpstrFile;
								
								bool bError = false;
								
								if( pSurSave ){
									RECT rc;
									rc.left		= 0;
									rc.top		= 0;
									rc.right	= desc.Width;
									rc.bottom	= desc.Height;
									if( D3DXSaveSurfaceToFile( ofn.lpstrFile , D3DXIFF_PNG , pSurSave , NULL , &rc ) ){
										bError = true;
									}
								} else bError = true;
								
								if( bError ){
									TCHAR buf[256];
									LoadString( (HINSTANCE)GetWindowLongPtr( hwnd , GWLP_HINSTANCE ) , IDS_STRING_ERR_SCREENSHOT , buf , 256 );
									MessageBox( hwnd , buf , TEXT("ScreenShot Error") , MB_ICONERROR | MB_OK );
								}
							}// if( GetSaveFileName( &ofn ) ){
							
							SAFE_RELEASE( pSurSave );
							SAFE_RELEASE( pTex );
						}
						break;
					
					default:
						break;
				}
			}
			break;
		
		case WM_RBUTTONUP:
			{ // �|�b�v�A�b�v���j���[��\��
				POINT pt; 
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);
				HMENU hmenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU_POPUP) );
				HMENU hSubmenu = GetSubMenu(hmenu, 0);
				// �`�F�b�N�}�[�N��ݒ肷��
				CheckMenuItem( hSubmenu , ID_MENU_TOPWINDOW , (g_AppConfig.m_bTopWindow?MF_CHECKED:MF_UNCHECKED) );
				CheckMenuItem( hSubmenu , ID_MENU_DROP_ERRFRAME , (g_AppConfig.m_bDropFrame?MF_CHECKED:MF_UNCHECKED) );
				
				ClientToScreen(hwnd, &pt);
				TrackPopupMenu(hSubmenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
				DestroyMenu(hmenu);
			}
			break;
		
		case WM_SIZE:
			{
				RECT rcC;
				_tprintd( TEXT("WM_SIZE start\n") );
				GetClientRect( hwnd , &rcC );
				long Cw , Ch;
				Cw = rcC.right - rcC.left;	Ch = rcC.bottom - rcC.top;
				_tprintd( TEXT("client %d %d\n") , Cw , Ch );
				
				if( g_pD3DDev ){
				if(g_mutex_Texture.lock(INFINITE) == false){
					D3DResQuit();
					D3DPRESENT_PARAMETERS d3dpp;
					D3DGetPParameters( &d3dpp , hwnd , Cw , Ch );
					g_pD3DDev->Reset( &d3dpp );
					
					D3DVIEWPORT9 vp;
					vp.X	= 0;
					vp.Y	= 0;
					vp.Width = d3dpp.BackBufferWidth;
					vp.Height = d3dpp.BackBufferHeight;
					vp.MinZ = 0.0f;
					vp.MaxZ = 0.0f;
					g_pD3DDev->SetViewport( &vp );
					
					D3DResInit( &g_pD3DDev );
					g_mutex_Texture.unlock();
				}
					//
				}

				_tprintd( TEXT("WM_SIZE end\n") );
			}
			break;
		
		case WM_PAINT:
			{
				BeginPaint( hwnd , &ps );
				EndPaint( hwnd , &ps );
			}
			break;
		
	}
	
//	_tprintd( TEXT("WindowProc end\n") );
//	ShowAppConfig();
	
	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

HWND InitWindow( HINSTANCE hInst , WNDPROC CallBack )
{
	WNDCLASS	WinClass;	//�E�B���h�E�̌`�̓o�^
	
	//0�ŏ�����
	ZeroMemory(&WinClass,sizeof(WNDCLASS));
	//�E�B���h�E�@�N���X��ݒ�
	WinClass.style			= CS_CLASSDC;								//�E�C���h�E�X�^�C����ݒ� 
	WinClass.lpfnWndProc	= (WNDPROC)CallBack;						//�R�[���o�b�N�v���V�[�W���ւ̃|�C���^
	WinClass.hInstance		= hInst;									//�C���X�^���X�n���h����ݒ�
	WinClass.hCursor		= LoadCursor(NULL, IDC_ARROW);				//�J�[�\���̐ݒ�(Windows�W�����\�[�X���g�p)
	WinClass.hbrBackground	= (HBRUSH)COLOR_WINDOW;						//�E�C���h�E�̔w�i��ݒ�(�f�t�H���g�J���[)
	WinClass.lpszClassName	= TEXT("Programming Library");					//�N���X���̐ݒ�
	WinClass.hIcon			= LoadIcon( hInst , MAKEINTRESOURCE(IDI_ICON1) );
	WinClass.lpszMenuName	= NULL;										//���j���[�̐ݒ�

	//�E�B���h�E�N���X�̓o�^
	if( RegisterClass( &WinClass ) == 0 )
	{
		MessageBox( NULL , TEXT("�o�^�G���[") , TEXT("�G���[") , 0 );
		return 0;
	}
	
	//���C���E�C���h�E�̐���
	TCHAR buf[256];
	LoadString( hInst , IDS_STRING_TITLE , buf , 256 );
	return CreateWindowEx(
		WS_EX_CONTROLPARENT | WS_EX_WINDOWEDGE ,	//�g���E�C���h�E�X�^�C��
		TEXT("Programming Library") ,				//�o�^���ꂽ�N���X���̃A�h���X 
		buf ,										//�E�C���h�E��
		WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME) ,		//�E�C���h�E�X�^�C����ݒ�(�ő剻�{�^���ƃT�C�Y�̕ύX�𖳌��ɂ���)
		CW_USEDEFAULT ,								//X���W�̈ʒu�ݒ�
		CW_USEDEFAULT ,								//Y���W�̈ʒu�ݒ�
		512 ,										//������ݒ�
		512 ,										//�c����ݒ�
		NULL ,										//�e�E�C���h�E��ݒ�
		LoadMenu( hInst , MAKEINTRESOURCE(IDR_MENU_MAIN) ) ,										//���j���[��ݒ�
		hInst ,										//�C���X�^���X������
		NULL );										//�쐬�����E�C���h�E�ɓn���f�[�^�ւ̃|�C���^
}

/*
	@fn void AppQuit( void )
	@brief �A�v���P�[�V�����̏I������
*/
void AppQuit( void )
{	
	g_thBuftoTEX.stopThread();
	g_mutex_Texture.destroy();
	D3DResQuit();
	D3DQuit( &g_pD3D , &g_pD3DDev );
	
	ConfigIOSave( &g_AppConfig , L"config.xml" );
	
	CoUninitialize();
}

/*!
	@brief ���C���֐�
*/
int WINAPI _tWinMain( HINSTANCE hInst , HINSTANCE hPrevInstance , LPTSTR lpszCmdLine , int nCmdShow )
{
	MSG			msg;		//���b�Z�[�W
	
	g_hInst = hInst;
	
	setlocale( LC_ALL, "Japanese" );
	
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		_tprintd( TEXT("CoInitialize Error\n") );
		return -1;
	}
	
	memset( &g_AppConfig , 0 , sizeof(g_AppConfig) );
	g_AppConfig.m_fScrScal = 1.0f;
	
	// config.xml����ݒ��ǂݍ���
	ConfigIOLoad( &g_AppConfig , L"config.xml" );
	
	// �R�}���h���C���̏���
	if( _tcslen(lpszCmdLine) )
		ConfigIOCheckCmdLine( &g_AppConfig , lpszCmdLine );
	
	// �g�p�n�[�h�E�F�A�̐ݒ�
	if (g_AppConfig.m_iconsole != g_console.GetConsole()){
		g_console.SetConsole(g_AppConfig.m_iconsole);
		change_console();
	}

	// �ݒ肩��X�N���[���T�C�Y���擾����
	RECT rc = { 0 , 0 , g_console.GetWidth() , g_console.GetHeight() };
	GetClientSizeForAppconfig( &g_AppConfig , &rc );
	
	// �E�B���h�E���쐬
	g_hWnd = InitWindow( hInst , WindowProc );
	
	{
		long fps = g_console.GetFps();
		if( g_AppConfig.m_eFrmSkip == ECAPFPS_60){ fps = g_console.GetFps(ECAPFPS_60);}
		if( g_AppConfig.m_eFrmSkip == ECAPFPS_30){ fps = g_console.GetFps(ECAPFPS_30);}
		if( g_AppConfig.m_eFrmSkip == ECAPFPS_20){ fps = g_console.GetFps(ECAPFPS_20);}
		if( g_AppConfig.m_eFrmSkip == ECAPFPS_15){ fps = g_console.GetFps(ECAPFPS_15);}
		
		g_thRender.setFps( fps );
		g_thBuftoTEX.setFps( fps );
	}
	
	// usb start
	if( g_nise.NisetroInit( g_AppConfig.m_eFrmSkip , g_AppConfig.m_iCUSB2_ID , g_hWnd ) )
		g_nise.NisetroQuit();
	
	// �G���[�t���[���̑Ώ���ݒ�
	g_nise.SetDropFrame(g_AppConfig.m_bDropFrame);
	
	g_mutex_Texture.create();
	g_thBuftoTEX.startThread();
	
	D3DInit( &g_pD3D , &g_pD3DDev , g_hWnd , rc.right , rc.bottom );
	D3DResInit( &g_pD3DDev );
	
	//�E�B���h�E�̕\��
	ShowWindow( g_hWnd, nCmdShow );
	UpdateWindow( g_hWnd );
	
	// �E�B���h�E�T�C�Y���X�V
	D3DUpdateScreenConfig( &g_AppConfig , g_hWnd );
	
	// �V���[�g�J�b�g�̐ݒ���擾
	HACCEL hAccel = LoadAccelerators( hInst,  MAKEINTRESOURCE(IDR_ACCELERATOR1) );
	
	// �`��X���b�h�̊J�n
	g_thRender.startThread();
	
	//���[�v�J�n
	while (GetMessage(&msg, NULL, 0, 0)) { 
		if (!TranslateAccelerator(msg.hwnd, hAccel, &msg)) {
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
	}
	
	g_thRender.stopThread();
	
	g_nise.NisetroQuit();
	
	AppQuit();
	return ((int)msg.wParam);
}

