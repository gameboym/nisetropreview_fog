
#include "nisetro.h"

#ifndef __CONFIGIO_H__
#define __CONFIGIO_H__

// 画面の向きに関する定数
#define DIRMODE_NORMAL		0		// 通常
#define DIRMODE_LEFT		1		// 左90度回転
#define DIRMODE_RIGHT		2		// 右90度回転
#define DIRMODE_KERORICAN	3		// ケロリカン用回転
#define DIRMODE_MAX			4		// 項目数

// ケロリカンの回転角度
#define DEGREE_KERORICAN	-26.5f	// 左30度回転

//! 拡大率の変動の割合
const float CSCRSCAL_SCALE = 10.0f;	// 10%ごと(嘘)

//! 拡大率の最小値
const float CSCRSCAL_MIN = 0.5f;

//! 拡大率の最大値
const float CSCRSCAL_MAX = 5.0f;

//! 設定情報を格納する型
typedef struct SAPPCONFIG {
	//! 画面の向き
	unsigned char	m_DirMode;

	//! 拡大率
	float			m_fScrScal;
	
	//! 表示フラグ(手前に表示する)
	bool			m_bTopWindow;

	//! エラーがあった場合、表示を更新しない
	bool			m_bDropFrame;
	
	// カメレオンUSBのＩＤ
	int				m_iCUSB2_ID;
	
	//! 表示する速度
	ECAPFPS			m_eFrmSkip;
	
	//! 使用ハードウェア
	int				m_iconsole;
} SAPPCONFIG , *PAPPCONFIG , *LPAPPCONFIG;

/*!
	@brief	設定からクライアントサイズを取得する
	@param	pAppConfig	[in] 設定変数へのポインタ
	@param	pRC			[out] クライアントサイズ
*/
void GetClientSizeForAppconfig( LPAPPCONFIG pAppConfig , RECT *pRC );

/*!
	@brief	デバック表示用関数
	@param	pAppConfig	[in] 表示する設定変数へのポインタ
	@note	表示方法は_printdに依存する。
	
*/
void ShowAppConfig( LPAPPCONFIG pAppConfig );

/*!
	@brief	設定をファイル(xml)に書き出す
	@param	pConfig		[in] 設定変数へのポインタ
	@param	pFilename	[in] ファイル名
*/
int ConfigIOSave( PAPPCONFIG pConfig , wchar_t* pFilename );

/*!
	@brief	設定ファイル(xml)から設定を読む
	@param	pConfig		[out] 設定変数へのポインタ
	@param	pFilename	[in] ファイル名
*/
int ConfigIOLoad( PAPPCONFIG pConfig , wchar_t* pFilename );

/*!
	@brief	WinMainからのコマンドラインから設定情報を取得する。
	@param	pConfig		[out] 設定変数へのポインタ
	@param	lpszCmdLine	[in] コマンドライン文字列
*/
int ConfigIOCheckCmdLine( PAPPCONFIG pConfig , LPTSTR lpszCmdLine );

#endif

