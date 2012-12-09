
//! 情報ダイアログ用のコールバック
BOOL CALLBACK InfoDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		// 初期化
		case WM_INITDIALOG:
			{
				SetTimer( hDlgWnd , 1421356 , (1000 / 2) , NULL );
			//
			}
			return TRUE;
		
		case WM_DESTROY:
			{
				KillTimer( hDlgWnd, 1421356 );
			}
			break;
		
		case WM_TIMER:
			{
				TCHAR strBuf[256];
				TCHAR strTitle[256];
				LoadString( (HINSTANCE)GetWindowLongPtr(hDlgWnd, GWLP_HINSTANCE) , IDS_STRING_TITLE , strTitle , 256 );
#if _MSC_VER >= 1400
				_stprintf_s( strBuf , 256 , TEXT("%s\r\nFPS %2.1f\r\nData %2.1f\r\nErrorFrame:%d") , strTitle , g_thRender.m_dFPS , g_nise.GetFps() , g_nise.GetErrorFrameCount() );
#else
				_stprintf( strBuf , TEXT("%s\r\nFPS %2.1f\r\nData %2.1f\r\nErrorFrame:%d") , strTitle , g_thRender.m_dFPS , g_nise.GetFps() , g_nise.GetErrorFrameCount() );
#endif
				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_INFO) , strBuf );
			}
			break;
		
		case WM_COMMAND:
			switch (LOWORD(wp)) {
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
		
		case WM_CLOSE:
			DestroyWindow(g_hInfoDlg);
			g_hInfoDlg = NULL;
//			EndDialog(hDlgWnd, IDCANCEL);
			break;
		
		default:
			return FALSE;
	}
	return FALSE;
} 

