#include "gameconsole.h"
extern CCONSOLEGAMES g_console;

//! 設定ダイアログ用のコールバック
BOOL CALLBACK ConfigDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		// 初期化
		case WM_INITDIALOG:
			{
				TCHAR buf[256];
				
				// 更新速度
				for( unsigned int i = 0; i < 4; i++ ){
					LoadString( g_hInst , IDS_STRING_60FPS+i , buf , 256);
					SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_ADDSTRING, 0, (LPARAM)buf);
				}
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_SETCURSEL, 0, 0 );
			
				// 画面の向き
				for( unsigned int i = 0; i < DIRMODE_MAX ; i++ ){
					LoadString( g_hInst , IDS_STRING_NORMAL+i , buf , 256);
					SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_SCRDIR), CB_ADDSTRING, 0, (LPARAM)buf);
				}
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_SCRDIR), CB_SETCURSEL, g_AppConfig.m_DirMode, 0 );
				
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_SCRSCALE), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELPARAM((CSCRSCAL_MIN*CSCRSCAL_SCALE), (CSCRSCAL_MAX*CSCRSCAL_SCALE)));
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_SCRSCALE), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)(g_AppConfig.m_fScrScal*CSCRSCAL_SCALE));
				SendMessage( hDlgWnd , WM_HSCROLL , 0 , (LPARAM)GetDlgItem(hDlgWnd, IDC_SLIDER_SCRSCALE) );
				
				if( g_AppConfig.m_eFrmSkip == ECAPFPS_60 )	SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_SETCURSEL, 0, 0 );
				if( g_AppConfig.m_eFrmSkip == ECAPFPS_30 )	SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_SETCURSEL, 1, 0 );
				if( g_AppConfig.m_eFrmSkip == ECAPFPS_20 )	SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_SETCURSEL, 2, 0 );
				if( g_AppConfig.m_eFrmSkip == ECAPFPS_15 )	SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_SETCURSEL, 3, 0 );
				
			}
			return TRUE;
		
		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_COMBO_SCRDIR:
					{
						if( HIWORD(wp) == CBN_SELCHANGE ){
							g_AppConfig.m_DirMode = (unsigned char)SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_SCRDIR), CB_GETCURSEL, 0,0);
							D3DUpdateScreenConfig( &g_AppConfig , g_hWnd );
						}
					}
					break;
				
				case IDC_COMBO_FPS:
					{
						if( HIWORD(wp) == CBN_SELCHANGE ){
							long fps = 0;
							long sel = (long)SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_FPS), CB_GETCURSEL, 0,0);
							if( sel == 0 ){	g_AppConfig.m_eFrmSkip = ECAPFPS_60; fps = g_console.GetFps(ECAPFPS_60);}
							if( sel == 1 ){	g_AppConfig.m_eFrmSkip = ECAPFPS_30; fps = g_console.GetFps(ECAPFPS_30);}
							if( sel == 2 ){	g_AppConfig.m_eFrmSkip = ECAPFPS_20; fps = g_console.GetFps(ECAPFPS_20);}
							if( sel == 3 ){	g_AppConfig.m_eFrmSkip = ECAPFPS_15; fps = g_console.GetFps(ECAPFPS_15);}
							
							g_thRender.setFps( fps );
							g_thBuftoTEX.setFps( fps );
							g_nise.NisetroQuit();
							g_nise.NisetroInit( g_AppConfig.m_eFrmSkip , g_AppConfig.m_iCUSB2_ID , g_hWnd );
						}
					}
					break;
				
				case IDOK:
					{
						SendMessage( hDlgWnd , WM_CLOSE , 0 , 0 );
					}
					return TRUE;
				case IDCANCEL:
					SendMessage( hDlgWnd , WM_CLOSE , 0 , 0 );
					return TRUE;
				default:
					return FALSE;
			}
		
		case WM_HSCROLL:
			{
				
				TCHAR buf[256];
				switch(GetWindowLongPtr((HWND)lp,GWL_ID)) {					
					// 拡大率
					case IDC_SLIDER_SCRSCALE:
						{
							int t = (int)SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_SCRSCALE), TBM_GETPOS, 0, 0);
							g_AppConfig.m_fScrScal = (float)t / CSCRSCAL_SCALE;
							if( g_AppConfig.m_fScrScal <= CSCRSCAL_MIN ) g_AppConfig.m_fScrScal = CSCRSCAL_MIN;
							if( g_AppConfig.m_fScrScal >= CSCRSCAL_MAX ) g_AppConfig.m_fScrScal = CSCRSCAL_MAX;
#if _MSC_VER >= 1400
							_stprintf_s( buf , 256 , TEXT("x%2.2f") , g_AppConfig.m_fScrScal );
#else
							_stprintf( buf , TEXT("x%2.2f") , g_AppConfig.m_fScrScal );
#endif
							SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_SCRSCALE) , buf );
						}
						break;
				}

				D3DUpdateScreenConfig( &g_AppConfig , g_hWnd );
				//
			}
			break;
		
		case WM_CLOSE:
			DestroyWindow(g_hConfigDlg);
			g_hConfigDlg = NULL;
//			EndDialog(hDlgWnd, IDCANCEL);
			break;
		
		default:
			return FALSE;
	}
	return FALSE;
} 

