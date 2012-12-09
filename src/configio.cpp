
#include <stdio.h>
#include <math.h>
#include <tchar.h>
#include "nisetro.h"
#include "configio.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define XTORAD( degree ) ((degree) * (M_PI / 180.0f))

extern "C" {
#include "_tprintd.h"
};

#include "gameconsole.h"
extern CCONSOLEGAMES g_console;

#import "msxml4.dll" raw_interfaces_only

using namespace MSXML2;

/*
	@brief	設定からクライアントサイズを取得する
	@param	pAppConfig	[in] 設定変数へのポインタ
	@param	pRC			[out] クライアントサイズ
*/
void GetClientSizeForAppconfig( LPAPPCONFIG pAppConfig , RECT *pRC ){
	if( pAppConfig == NULL ) return;
	if( pRC == NULL ) return;
	pRC->left	= 0;
	pRC->top	= 0;

	pRC->right = g_console.GetWidth();
	pRC->bottom = g_console.GetHeight();
	
	// 幅と高さを入れ替える
	if( (pAppConfig->m_DirMode == DIRMODE_LEFT) || (pAppConfig->m_DirMode == DIRMODE_RIGHT) ){
		long t = pRC->right;
		pRC->right = pRC->bottom;
		pRC->bottom = t;
	}
	else if( pAppConfig->m_DirMode == DIRMODE_KERORICAN ){
		double deg = DEGREE_KERORICAN;
		pRC->right	= (long) ( fabs( g_console.GetWidth() * cos(XTORAD(deg)) ) + fabs( g_console.GetHeight() * sin(XTORAD(deg)) ) + 0.5);
		pRC->bottom	= (long) ( fabs( g_console.GetWidth() * sin(XTORAD(deg)) ) + fabs( g_console.GetHeight() * cos(XTORAD(deg)) ) + 0.5);
	}
	
	pRC->right = (long)((float)pRC->right * pAppConfig->m_fScrScal);
	pRC->bottom = (long)((float)pRC->bottom * pAppConfig->m_fScrScal);
	
}

void ShowAppConfig( LPAPPCONFIG pAppConfig ){
	if( pAppConfig == NULL ) return;
	_tprintd( TEXT("scale\t%f\n") , pAppConfig->m_fScrScal );
	
	if( pAppConfig->m_DirMode == DIRMODE_NORMAL ) _tprintd( TEXT("dir  \t%s\n") , TEXT("Normal") );
	else if( pAppConfig->m_DirMode == DIRMODE_LEFT ) _tprintd( TEXT("dir  \t%s\n") , TEXT("left") );
	else if( pAppConfig->m_DirMode == DIRMODE_RIGHT ) _tprintd( TEXT("dir  \t%s\n") , TEXT("right") );
	else if( pAppConfig->m_DirMode == DIRMODE_KERORICAN ) _tprintd( TEXT("dir  \t%s\n") , TEXT("kerorican") );
	else _tprintd( TEXT("dir  \t%d\n") , pAppConfig->m_DirMode );
}

/*
	@brief	設定をファイル(xml)に書き出す
	@param	pConfig		[in] 設定変数へのポインタ
	@param	pFilename	[in] ファイル名
*/
int ConfigIOSave( PAPPCONFIG pConfig , wchar_t* pFilename ){
	if( pConfig == NULL || pFilename == NULL ) return -1;
	
	MSXML2::IXMLDOMDocument2Ptr pDoc;
	MSXML2::IXMLDOMProcessingInstructionPtr pPI;
	MSXML2::IXMLDOMElementPtr	pEmtConfig;
	MSXML2::IXMLDOMElementPtr	pElement;
	MSXML2::IXMLDOMTextPtr		pEmtBLine;
	wchar_t strBuf[256];
	
	try{
		pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		pDoc->put_async( VARIANT_FALSE );
		
		pDoc->createProcessingInstruction( L"xml", L"version=\'1.0\' encoding=\'UTF-8\'" , &pPI );
		pDoc->appendChild( pPI , NULL );
		pPI = NULL;
		
		// <config>を作成
		pDoc->createElement(L"config", &pEmtConfig);
		
		pDoc->createTextNode(L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		
		// <frameskip>を作成
		pDoc->createTextNode(L"\t" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		pDoc->createElement(L"frameskip", &pElement);
		swprintf( strBuf , 256 , L"%d" , pConfig->m_eFrmSkip );
		pElement->put_text( strBuf );
		// <config>に<frameskip>を追加する
		pEmtConfig->appendChild(pElement, NULL);
		pElement = NULL;
		pDoc->createTextNode(L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		
		// <dir>を作成
		pDoc->createTextNode(L"\t" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		pDoc->createElement(L"dir", &pElement);
		swprintf( strBuf , 256 , L"%d" , pConfig->m_DirMode );
		pElement->put_text( strBuf );
		// <config>に<dir>を追加する
		pEmtConfig->appendChild(pElement, NULL);
		pElement = NULL;
		pDoc->createTextNode( L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		
		// <scale>を作成
		pDoc->createTextNode( L"\t" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		pDoc->createElement( L"scale", &pElement);
		swprintf( strBuf , 256 , L"%f" , pConfig->m_fScrScal );
		pElement->put_text( strBuf );
		// <config>に<scale>を追加する
		pEmtConfig->appendChild(pElement, NULL);
		pElement = NULL;
		pDoc->createTextNode( L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		
		// <topwindow>を作成
		pDoc->createTextNode( L"\t" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		pDoc->createElement( L"topwindow", &pElement);
		swprintf( strBuf , 256 , L"%d" , pConfig->m_bTopWindow );
		pElement->put_text( strBuf );
		// <config>に<topwindow>を追加する
		pEmtConfig->appendChild(pElement, NULL);
		pElement = NULL;
		pDoc->createTextNode( L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		
		// <dropframe>を作成
		pDoc->createTextNode( L"\t" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		pDoc->createElement( L"dropframe", &pElement);
		swprintf( strBuf , 256 , L"%d" , pConfig->m_bDropFrame );
		pElement->put_text( strBuf );
		// <config>に<dropframe>を追加する
		pEmtConfig->appendChild(pElement, NULL);
		pElement = NULL;
		pDoc->createTextNode( L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;

		// <console>を作成
		pDoc->createTextNode( L"\t" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;
		pDoc->createElement( L"console", &pElement);
		swprintf( strBuf , 256 , L"%d" , pConfig->m_iconsole );
		pElement->put_text( strBuf );
		// <config>に<console>を追加する
		pEmtConfig->appendChild(pElement, NULL);
		pElement = NULL;
		pDoc->createTextNode( L"\n" , &pEmtBLine );
		pEmtConfig->appendChild( pEmtBLine , NULL ); // 改行
		pEmtBLine = NULL;

		// <config> をドキュメントに追加する
		pDoc->appendChild(pEmtConfig, NULL);
		pEmtConfig = NULL;
		
		_variant_t strOutPath(::SysAllocString(pFilename));
		pDoc->save(strOutPath);
		
	}catch(_com_error &e){
		_tprintd( TEXT("xml error %s\n") , e.Description() );
	}
	
	pEmtBLine = NULL;
	pElement = NULL;
	pEmtConfig = NULL;
	pPI = NULL;
	pDoc = NULL;

	_tprintd( TEXT("config save end\n") );
	
	return 0;
}

/*
	@brief	設定ファイル(xml)から設定を読む
	@param	pConfig		[out] 設定変数へのポインタ
	@param	pFilename	[in] ファイル名
*/
int ConfigIOLoad( PAPPCONFIG pConfig , wchar_t* pFilename ){
	if( pConfig == NULL || pFilename == NULL ) return -1;
	
	MSXML2::IXMLDOMDocument2Ptr pDoc;
	MSXML2::IXMLDOMElementPtr pRoot;
	MSXML2::IXMLDOMNodePtr pNodeRet;
	VARIANT_BOOL isSuc;
	
	try{
		pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		pDoc->put_async(VARIANT_FALSE);
		pDoc->load(_variant_t(pFilename) , &isSuc );
		pDoc->get_documentElement(&pRoot);
				
		pRoot->selectSingleNode( L"frameskip" , &pNodeRet );
		if( pNodeRet ){
			BSTR    value;
			pNodeRet->get_text( &value );
			long v = _wtol(value);
			_tprintd( TEXT("frameskip %d\n") , v );
			if( v == ECAPFPS_60 || v == ECAPFPS_30 || v == ECAPFPS_20 || v == ECAPFPS_15 )
				pConfig->m_eFrmSkip = (ECAPFPS)v;
			pNodeRet = NULL;
		}
		
		pRoot->selectSingleNode( L"dir" , &pNodeRet );
		if( pNodeRet ){
			BSTR    value;
			pNodeRet->get_text( &value );
			long v = _wtol(value);
			_tprintd( TEXT("dir %d\n") , v );
			if( v >= 0 && v < DIRMODE_MAX )
				pConfig->m_DirMode = (unsigned char)v;
			pNodeRet = NULL;
		}

		pRoot->selectSingleNode( L"scale" , &pNodeRet );
		if( pNodeRet ){
			BSTR    value;
			pNodeRet->get_text( &value );
			float v = (float)_wtof(value);
			_tprintd( TEXT("scale %f\n") , v );
			if( v <= CSCRSCAL_MIN ) v = CSCRSCAL_MIN;
			if( v >= CSCRSCAL_MAX ) v = CSCRSCAL_MAX;
			pConfig->m_fScrScal = v;
			pNodeRet = NULL;
		}
		
		pRoot->selectSingleNode( L"topwindow" , &pNodeRet );
		if( pNodeRet ){
			BSTR    value;
			pNodeRet->get_text( &value );
			long v = _wtol(value);
			_tprintd( TEXT("topwindow %d\n") , v );
			pConfig->m_bTopWindow = (v ? true : false );
			pNodeRet = NULL;
		}
		
		pRoot->selectSingleNode( L"dropframe" , &pNodeRet );
		if( pNodeRet ){
			BSTR    value;
			pNodeRet->get_text( &value );
			long v = _wtol(value);
			_tprintd( TEXT("dropframe %d\n") , v );
			pConfig->m_bDropFrame = (v ? true : false );
			pNodeRet = NULL;
		}
		
		pRoot->selectSingleNode( L"console" , &pNodeRet );
		if( pNodeRet ){
			BSTR    value;
			pNodeRet->get_text( &value );
			long v = _wtol(value);
			_tprintd( TEXT("console %d\n") , v );
			if( v < 0 ) v = 0;
			if( v >= MAX_CONSOLE_NUM ) v = MAX_CONSOLE_NUM-1;
			pConfig->m_iconsole = (int)v;
			pNodeRet = NULL;
		}
	}catch(_com_error &e){
		_tprintd( TEXT("xml error %s\n") , e.Description() );
	}
	
	pNodeRet	= NULL;
	pRoot		= NULL;
	pDoc		= NULL;
	//
	_tprintd( TEXT("config load end\n") );
	
	return 0;
}

int ConfigIOCheckCmdLine( PAPPCONFIG pConfig , LPTSTR lpszCmdLine ){
	if( pConfig == NULL || lpszCmdLine == NULL ) return -1;
	
		LPTSTR pStr = NULL;
		
#if _MSC_VER >= 1400
		LPTSTR pStrNext = NULL;
		pStr = _tcstok_s( lpszCmdLine , TEXT(" ") , &pStrNext );
#else
		pStr = _tcstok( lpszCmdLine , TEXT(" "));
#endif
		while( pStr ){
			
			// カメレオンusbのＩＤ
			if( _tcscmp( pStr , TEXT("--cusb2id") ) == 0 ){
#if _MSC_VER >= 1400
				pStr = _tcstok_s( NULL , TEXT(" ") , &pStrNext );
#else
				pStr = _tcstok( NULL , TEXT(" ") );
#endif
				pConfig->m_iCUSB2_ID = _tstol(pStr);
				pConfig->m_iCUSB2_ID = pConfig->m_iCUSB2_ID;	// break point
			}
			
			// 使用ハードウェア
			if( _tcscmp( pStr , TEXT("--console") ) == 0 ){
#if _MSC_VER >= 1400
				pStr = _tcstok_s( NULL , TEXT(" ") , &pStrNext );
#else
				pStr = _tcstok( NULL , TEXT(" ") );
#endif
				pConfig->m_iconsole = _tstol(pStr);
				if( pConfig->m_iconsole < 0 ) pConfig->m_iconsole = 0;
				if( pConfig->m_iconsole >= MAX_CONSOLE_NUM ) pConfig->m_iconsole = MAX_CONSOLE_NUM-1;
			}

			// 画面の拡大率
			if( _tcscmp( pStr , TEXT("--scale") ) == 0 ){
#if _MSC_VER >= 1400
				pStr = _tcstok_s( NULL , TEXT(" ") , &pStrNext );
#else
				pStr = _tcstok( NULL , TEXT(" ") );
#endif
				pConfig->m_fScrScal = (float)_tstof(pStr);
				if( pConfig->m_fScrScal <= CSCRSCAL_MIN ) pConfig->m_fScrScal = CSCRSCAL_MIN;
				if( pConfig->m_fScrScal >= CSCRSCAL_MAX ) pConfig->m_fScrScal = CSCRSCAL_MAX;
				pConfig->m_fScrScal = pConfig->m_fScrScal;	// break point
			}
			
			// ドロップフレーム
			if( _tcscmp( pStr , TEXT("--dropframe") ) == 0 ){
				pConfig->m_bDropFrame = true;
				pConfig->m_bDropFrame = pConfig->m_bDropFrame;	// break point
			}
			
			// 常に手前に表示
			if( _tcscmp( pStr , TEXT("--topwindow") ) == 0 ){
				pConfig->m_bTopWindow = true;
				pConfig->m_bTopWindow = pConfig->m_bTopWindow;	// break point
			}
			
			// 左回転
			if( _tcscmp( pStr , TEXT("--rotation-left") ) == 0 ){
				pConfig->m_DirMode = DIRMODE_LEFT;
				pConfig->m_DirMode = pConfig->m_DirMode;	// break point
			}
			
			// 右回転
			if( _tcscmp( pStr , TEXT("--rotation-right") ) == 0 ){
				pConfig->m_DirMode = DIRMODE_RIGHT;
				pConfig->m_DirMode = pConfig->m_DirMode;	// break point
			}

			// ケロリカン用回転
			if( _tcscmp( pStr , TEXT("--rotation-kerorican") ) == 0 ){
				pConfig->m_DirMode = DIRMODE_KERORICAN;
				pConfig->m_DirMode = pConfig->m_DirMode;	// break point
			}

			// フレームスキップ(60fps (WS 75fps))
			if( _tcscmp( pStr , TEXT("--frameskip-60fps") ) == 0 ){
				pConfig->m_eFrmSkip = ECAPFPS_60;
				pConfig->m_eFrmSkip = pConfig->m_eFrmSkip;	// break point
			}
			
			// フレームスキップ(30fps (WS 60fps))
			if( _tcscmp( pStr , TEXT("--frameskip-30fps") ) == 0 ){
				pConfig->m_eFrmSkip = ECAPFPS_30;
				pConfig->m_eFrmSkip = pConfig->m_eFrmSkip;	// break point
			}
			
			// フレームスキップ(20fps (WS 30fps))
			if( _tcscmp( pStr , TEXT("--frameskip-20fps") ) == 0 ){
				pConfig->m_eFrmSkip = ECAPFPS_20;
				pConfig->m_eFrmSkip = pConfig->m_eFrmSkip;	// break point
			}
		
			// フレームスキップ(15fps (WS 15fps))
			if( _tcscmp( pStr , TEXT("--frameskip-15fps") ) == 0 ){
				pConfig->m_eFrmSkip = ECAPFPS_15;
				pConfig->m_eFrmSkip = pConfig->m_eFrmSkip;	// break point
			}
			
#if _MSC_VER >= 1400
				pStr = _tcstok_s( NULL , TEXT(" ") , &pStrNext );
#else
				pStr = _tcstok( NULL , TEXT(" ") );
#endif
		}
	
	
	
	return 0;
}

