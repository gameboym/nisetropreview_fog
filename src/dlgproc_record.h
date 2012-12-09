const WORD FPS_MIN	= 1;
const WORD FPS_MAX	= 100;

const WORD VKBITRATE_MIN	= 1;
const WORD VKBITRATE_MAX	= 16 * 1024; // 適当に設定

const WORD ABITRATE_MIN	= 32;
const WORD ABITRATE_MAX	= 1024;

TCHAR *g_pCodecList[] = {
	TEXT("mpeg4"),
	TEXT("mp2"),
	TEXT("pcm_u32be"),
	TEXT("ac3"),
	TEXT("mpeg1video"),
	TEXT("pcm_u32le"),
	TEXT("adpcm_adx"),
	TEXT("mpeg2video"),
	TEXT("pcm_u8"),
	TEXT("adpcm_g726"),
	TEXT("pcm_zork"),
	TEXT("adpcm_ima_qt"),
	TEXT("msmpeg4v1"),
	TEXT("pcx"),
	TEXT("adpcm_ima_wav"),
	TEXT("msmpeg4v2"),
	TEXT("pgm"),
	TEXT("adpcm_ms"),
	TEXT("msmpeg4v3"),
	TEXT("pgmyuv"),
	TEXT("adpcm_swf"),
	TEXT("nellymoser"),
	TEXT("ppm"),
	TEXT("adpcm_yamaha"),
	TEXT("pam"),
	TEXT("qtrle"),
	TEXT("alac"),
	TEXT("pbm"),
	TEXT("rawvideo"),
	TEXT("asv1"),
	TEXT("roq"),
	TEXT("asv2"),
	TEXT("roq_dpcm"),
	TEXT("bmp"),
	TEXT("rv10"),
	TEXT("dnxhd"),
	TEXT("rv20"),
	TEXT("dvbsub"),
	TEXT("sgi"),
	TEXT("dvdsub"),
	TEXT("snow"),
	TEXT("dvvideo"),
	TEXT("sonic"),
	TEXT("ffv1"),
	TEXT("sonic_ls"),
	TEXT("ffvhuff"),
	TEXT("svq1"),
	TEXT("flac"),
	TEXT("targa"),
	TEXT("flv"),
	TEXT("tiff"),
	TEXT("gif"),
	TEXT("v210"),
	TEXT("h261"),
	TEXT("h263"),
	TEXT("h263p"),
	TEXT("huffyuv"),
	TEXT("wmv1"),
	TEXT("jpegls"),
	TEXT("wmv2"),
	TEXT("ljpeg"),
	TEXT("xsub"),
	TEXT("mjpeg"),
	TEXT("vorbis"),
	TEXT("pcm_alaw"),
	TEXT("pcm_f32be"),
	TEXT("pcm_f32le"),
	TEXT("pcm_f64be"),
	TEXT("pcm_f64le"),
	TEXT("pcm_mulaw"),
	TEXT("pcm_s16be"),
	TEXT("pcm_s16le"),
	TEXT("pcm_s24be"),
	TEXT("pcm_s24daud"),
	TEXT("pcm_s24le"),
	TEXT("pcm_s32be"),
	TEXT("pcm_s32le"),
	TEXT("pcm_s8"),
	TEXT("pcm_u16be"),
	TEXT("pcm_u16le"),
	TEXT("pcm_u24be"),
	TEXT("pcm_u24le"),
	TEXT("wmav1"),
	TEXT("wmav2"),
	NULL
};

TCHAR *g_pASampleList[] = {
//	TEXT("8000"),
//	TEXT("11025"),
	TEXT("22050"),
	TEXT("32000"),
//	TEXT("33075"),
	TEXT("44100"),
//	TEXT("47250"),
	TEXT("48000"),
/*	TEXT("96000"),
	TEXT("192000"),
	TEXT("14112000"),
	TEXT("28224000"),
	TEXT("56448000"),*/
	NULL
};

//! 録画ダイアログ用のコールバック
BOOL CALLBACK RecordDlgProc(HWND hDlgWnd, UINT msg, WPARAM wp, LPARAM lp) {
	static long lFps = 24;
	static long lVKBitRate = 6 * 1024;
	static long lAKBitRate = 64;
	static long lAChannels = 2;
	static long lASamplesPerSec = 44100;
	
	switch (msg) {
		// 初期化
		case WM_INITDIALOG:
			{
				// WaveInデバイスを列挙する
				unsigned int c = waveInGetNumDevs();
				for( unsigned int i = 0; i < c; i++ ){
					WAVEINCAPS wCaps;
					waveInGetDevCaps( (UINT_PTR)i , &wCaps , sizeof(WAVEINCAPS) );
					SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_ADDSTRING, 0, (LPARAM)wCaps.szPname );
				}
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_ADDSTRING, 0, (LPARAM)TEXT("WAVE_MAPPER"));
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_SETCURSEL, c, 0 );
				
				// ファイル名を設定する
				TCHAR buf[256];
				LoadString( g_hInst , IDS_STRING_DEF_CAPNAME , buf , 256 );
				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_FILENAME) , buf );
				
				// FPS
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_FPS), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELPARAM(FPS_MIN, FPS_MAX));
#if _MSC_VER >= 1400
				_stprintf_s( buf , 256 , TEXT("%d fps") , lFps );
#else
				_stprintf( buf , TEXT("%d fps") , lFps );
#endif
				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_FPS) , buf );
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_FPS), TBM_SETPOS, (WPARAM)TRUE, lFps );
				
				
				// VBitRate
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_VBITRATE), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELPARAM(VKBITRATE_MIN, VKBITRATE_MAX));
#if _MSC_VER >= 1400
				_stprintf_s( buf , 256 , TEXT("%d Kbps") , lVKBitRate );
#else
				_stprintf( buf , TEXT("%d Kbps") , lVKBitRate );
#endif
				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_VBITRATE) , buf );
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_VBITRATE), TBM_SETPOS, (WPARAM)TRUE, lVKBitRate );
				
				// ABitRate
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_ABITRATE), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELPARAM(ABITRATE_MIN, ABITRATE_MAX));
#if _MSC_VER >= 1400
				_stprintf_s( buf , 256 , TEXT("%d Kbps") , lAKBitRate );
#else
				_stprintf( buf , TEXT("%d Kbps") , lAKBitRate );
#endif
				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_ABITRATE) , buf );
				SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_ABITRATE), TBM_SETPOS, (WPARAM)TRUE, lAKBitRate );
				
				//
				for( int i = 0; g_pASampleList[i] != NULL; i++ ){
					SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_SAMPLE_RATE), CB_ADDSTRING, 0, (LPARAM)g_pASampleList[i] );
				}
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_SAMPLE_RATE), CB_SETCURSEL, 0, 0 );
				
				// コーディックリストを更新
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_VCODEC), CB_ADDSTRING, 0, (LPARAM)TEXT("- default -") );
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_ACODEC), CB_ADDSTRING, 0, (LPARAM)TEXT("- default -") );
				for( int i = 0; g_pCodecList[i] != NULL; i++ ){
					char strCodec[256];
					memset( strCodec , 0 , sizeof(char) * 256 );
#if defined(_UNICODE)
	#if _MSC_VER >= 1400
					wcstombs_s( NULL , strCodec , sizeof(char) * 256 , g_pCodecList[i] , _TRUNCATE );
	#else
					wcstombs( strCodec , g_pCodecList[i] , sizeof(char) * 256 );
	#endif
#else
	#if _MSC_VER >= 1400
					strcpy_s( strCodec , 256 , g_pCodecList[i] );
	#else
					strcpy( strCodec , g_pCodecList[i] );
	#endif
#endif
					AVCodec *codec = avcodec_find_encoder_by_name( strCodec );
					if( codec ){
						// コーディックを追加する
						if( codec->type == CODEC_TYPE_VIDEO )
							SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_VCODEC), CB_ADDSTRING, 0, (LPARAM)g_pCodecList[i] );
						if( codec->type == CODEC_TYPE_AUDIO )
							SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_ACODEC), CB_ADDSTRING, 0, (LPARAM)g_pCodecList[i] );
					}
				}
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_VCODEC), CB_SETCURSEL, 0, 0 );
				SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_ACODEC), CB_SETCURSEL, 0, 0 );
				
				//
//				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_TIME) , TEXT("00 : 00 : 00 . 00") );
				SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_TIME) , TEXT("time") );
			}
			return TRUE;
		
		case WM_HSCROLL:
			{
				
				TCHAR buf[256];
				switch(GetWindowLongPtr((HWND)lp,GWLP_ID)) {
					// fps
					case IDC_SLIDER_FPS:
						{
							lFps = (int)SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_FPS), TBM_GETPOS, 0, 0);
							if( lFps <= FPS_MIN ) lFps = FPS_MIN;
							if( lFps >= FPS_MAX ) lFps = FPS_MAX;
#if _MSC_VER >= 1400
							_stprintf_s( buf , 256 , TEXT("%d fps") , lFps );
#else
							_stprintf( buf , TEXT("%d px") , lFps );
#endif
							SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_FPS) , buf );
						}
						break;
					
					// VBitRate
					case IDC_SLIDER_VBITRATE:
						{
							lVKBitRate = (int)SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_VBITRATE), TBM_GETPOS, 0, 0);
							if( lVKBitRate <= VKBITRATE_MIN ) lVKBitRate = VKBITRATE_MIN;
							if( lVKBitRate >= VKBITRATE_MAX ) lVKBitRate = VKBITRATE_MAX;
#if _MSC_VER >= 1400
							_stprintf_s( buf , 256 , TEXT("%d Kbps") , lVKBitRate );
#else
							_stprintf( buf , TEXT("%d Kbps") , lVKBitRate );
#endif
							SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_VBITRATE) , buf );
						}
						break;
					
					// ABitRate
					case IDC_SLIDER_ABITRATE:
						{
							lAKBitRate = (int)SendMessage(GetDlgItem(hDlgWnd, IDC_SLIDER_ABITRATE), TBM_GETPOS, 0, 0);
							if( lAKBitRate <= ABITRATE_MIN ) lAKBitRate = ABITRATE_MIN;
							if( lAKBitRate >= ABITRATE_MAX ) lAKBitRate = ABITRATE_MAX;
#if _MSC_VER >= 1400
							_stprintf_s( buf , 256 , TEXT("%d Kbps") , lAKBitRate );
#else
							_stprintf( buf , TEXT("%d Kbps") , lAKBitRate );
#endif
							SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_ABITRATE) , buf );
						}
						break;
					
					// 
					default:
						{
						}
						break;
				}
				//
			}
			break;
		
		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDC_BTN_RECORD:
					{
						HMENU hMenu = GetMenu( g_hWnd );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_BTN_RECORD) , FALSE );
						if( g_thAviSave.IsRec() ){
							g_thAviSave.Stop();
							g_thAviSave.Quit();
							//CheckMenuItem( hMenu , ID_MENU_REC , (g_nise.pRecfile?MF_CHECKED:MF_UNCHECKED) );
							SetWindowText(GetDlgItem(hDlgWnd, IDC_BTN_RECORD) , TEXT("録画") );
						}else{
							TCHAR strMovieName[MAX_PATH];
							char strVCodecName[256];
							CodecID vcodec_id = CODEC_ID_NONE;
							char strACodecName[256];
							CodecID acodec_id = CODEC_ID_NONE;
							
							UINT uID = WAVE_MAPPER;
							UINT uIDMax = waveInGetNumDevs();
							
							GetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_FILENAME) , strMovieName , MAX_PATH );
							
							// Video Codec
							GetWindowTextA( GetDlgItem(hDlgWnd, IDC_COMBO_VCODEC) , strVCodecName , 256 );
							AVCodec *vcodec = avcodec_find_encoder_by_name( strVCodecName );
							if( vcodec )
								if( vcodec->type == CODEC_TYPE_VIDEO )
									vcodec_id = vcodec->id;
							
							// Audio Codec
							GetWindowTextA( GetDlgItem(hDlgWnd, IDC_COMBO_ACODEC) , strACodecName , 256 );
							AVCodec *acodec = avcodec_find_encoder_by_name( strACodecName );
							if( acodec )
								if( acodec->type == CODEC_TYPE_AUDIO )
									acodec_id = acodec->id;
							
							uID = (UINT)SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_GETCURSEL, 0, 0 );
							if( uID >= uIDMax )
								uID = WAVE_MAPPER;
							
							// lASamplesPerSec
							{
								TCHAR strSamplesPerSec[MAX_PATH];
								long tASamplesPerSec = 0;
								GetWindowText( GetDlgItem(hDlgWnd, IDC_COMBO_SAMPLE_RATE) , strSamplesPerSec , 256 );
								tASamplesPerSec = _ttol( strSamplesPerSec );
								if( tASamplesPerSec != 0 )
									lASamplesPerSec = tASamplesPerSec;
							}
							
							g_thAviSave.Init( strMovieName , 
								CODEC_ID_NONE , NDS_SCREEN_W , NDS_SCREEN2_H , lFps , lVKBitRate * 1024 , 
								uID , acodec_id , lAChannels , lASamplesPerSec , lAKBitRate * 1000 );
							if( g_thAviSave.IsInit() ){
								_ffmpeg_struct* param = g_thAviSave.GetFFMpeg();
								//	ファイル情報を表示する
								std::string str;
								str = "";
								if( param->m_pFmt ){
//									if( param->m_pFmt->name )		{str += param->m_pFmt->name; str += "\r\n";	}
									if( param->m_pFmt->long_name )	{str += param->m_pFmt->long_name; str += "\r\n";	}
//									if( param->m_pFmt->mime_type )	{str += param->m_pFmt->mime_type; str += "\r\n";	}
//									if( param->m_pFmt->extensions )	{str += param->m_pFmt->extensions; str += "\r\n";	}
								}
								
								if( param->m_pOutFmtContext ){
									if( param->m_pOutFmtContext->filename )		{str += param->m_pOutFmtContext->filename; str += "\r\n";	}
/*#if LIBAVFORMAT_VERSION_INT < (53<<16)
									if( param->m_pOutFmtContext->title )		{str += param->m_pOutFmtContext->title; str += "\r\n";	}
									if( param->m_pOutFmtContext->author )		{str += param->m_pOutFmtContext->author; str += "\r\n";	}
									if( param->m_pOutFmtContext->copyright )	{str += param->m_pOutFmtContext->copyright; str += "\r\n";	}
									if( param->m_pOutFmtContext->comment )		{str += param->m_pOutFmtContext->comment; str += "\r\n";	}
									if( param->m_pOutFmtContext->album )		{str += param->m_pOutFmtContext->album; str += "\r\n";	}
#endif*/
								}
								if( param->m_pVStream && param->m_pVStream->codec->codec->long_name )	{str += param->m_pVStream->codec->codec->long_name; str += "\r\n";}
								if( param->m_pAStream && param->m_pAStream->codec->codec->long_name )	{str += param->m_pAStream->codec->codec->long_name; str += "\r\n";}
								
								SetWindowTextA( GetDlgItem(hDlgWnd, IDC_EDIT_FILEINFO) , str.c_str() );
								
								// 録画を開始する
								g_thAviSave.Start();
								SetWindowText(GetDlgItem(hDlgWnd, IDC_BTN_RECORD) , TEXT("停止") );
								
							}else{
								// Init Error 
								TCHAR strBuf[256];
								LoadString( (HINSTANCE)GetWindowLongPtr(hDlgWnd, GWLP_HINSTANCE) , IDS_STRING_ERR_RECORD , strBuf , 256 );
								MessageBox( hDlgWnd , strBuf , TEXT("Record Error") , MB_OK | MB_ICONERROR );
								_tprintd( TEXT("error\n") );
							}
						}
						
						//
						EnableWindow( GetDlgItem(hDlgWnd, IDC_BTN_RECORD) , TRUE );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_BTN_UPDATE) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_BTN_FILESEL) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDCLOSE) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_SLIDER_FPS) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_SLIDER_VBITRATE) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_SLIDER_ABITRATE) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_COMBO_VCODEC) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						EnableWindow( GetDlgItem(hDlgWnd, IDC_COMBO_ACODEC) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
//						EnableWindow( GetDlgItem(hDlgWnd, IDC_COMBO_SAMPLE_RATE) , (!g_thAviSave.IsRec()?TRUE:FALSE) );
						
						EnableMenuItem( hMenu , ID_MENU_REC , (g_thAviSave.IsRec()?MF_GRAYED:MF_UNCHECKED) );
						
					}
					return TRUE;
				
				case IDC_BTN_UPDATE:
					{
						// 一覧を削除する
						while(SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_GETCOUNT, 0, 0) != 0)
							SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_DELETESTRING, 0, 0);
						
						// WaveInデバイスを列挙する
						unsigned int c = waveInGetNumDevs();
						for( unsigned int i = 0; i < c; i++ ){
							WAVEINCAPS wCaps;
							waveInGetDevCaps( (UINT_PTR)i , &wCaps , sizeof(WAVEINCAPS) );
							SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_ADDSTRING, 0, (LPARAM)wCaps.szPname );
						}
						SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_ADDSTRING, 0, (LPARAM)TEXT("WAVE_MAPPER"));
						SendMessage(GetDlgItem(hDlgWnd, IDC_COMBO_WAVEINDEV), CB_SETCURSEL, c, 0 );
						
					}
					return TRUE;
				
				case IDC_BTN_FILESEL:
					{
						OPENFILENAME ofn;
						TCHAR strFilename[MAX_PATH];
						TCHAR strFilter[256];
						TCHAR strExt[256];
						
						memset( &ofn , 0 , sizeof(OPENFILENAME) );
						memset( strFilename , 0 , sizeof(TCHAR) * MAX_PATH );
						memset( strFilter , 0 , sizeof(TCHAR) * 256 );
						memset( strExt , 0 , sizeof(TCHAR) * 256 );
						
						LoadString( (HINSTANCE)GetWindowLongPtr(hDlgWnd, GWLP_HINSTANCE) , IDS_STRING_DEF_FILTER , strFilter , 256 );
						LoadString( (HINSTANCE)GetWindowLongPtr(hDlgWnd, GWLP_HINSTANCE) , IDS_STRING_DEF_EXT , strExt , 256 );
						
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner	= hDlgWnd;
						ofn.lpstrFile	= strFilename;
						ofn.nMaxFile	= MAX_PATH;
						ofn.lpstrFilter	= strFilter;//TEXT("*.avi\0*.avi\0\0");
						ofn.lpstrDefExt	= strExt;//TEXT("avi\0\0");
						ofn.hInstance	= (HINSTANCE)GetWindowLongPtr( hDlgWnd , GWLP_HINSTANCE );
						ofn.Flags		= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
						
						if( GetSaveFileName(&ofn) != 0 ){
							SetWindowText( GetDlgItem(hDlgWnd, IDC_EDIT_FILENAME) , strFilename );
						}
					}
					return TRUE;
				
				case IDCLOSE:
						SendMessage( hDlgWnd , WM_CLOSE , 0 , 0 );
					return TRUE;
				default:
					return FALSE;
			}
		
		case WM_CLOSE:
			
			if( g_thAviSave.IsRec() ){
				g_thAviSave.Stop();
				g_thAviSave.Quit();
			}
			if( g_hRecordDlg ){
				DestroyWindow(g_hRecordDlg);
				g_hRecordDlg = NULL;
			}
//			EndDialog(hDlgWnd, IDCANCEL);
			break;
		
		default:
			return FALSE;
	}
	return FALSE;
}

