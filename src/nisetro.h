#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "cusb2.h"
#include "cmutex.h"

#ifndef __NISETROINFO_H__
#define __NISETROINFO_H__
// CUSB2 に送るコマンドらしい
#define CMD_EP6IN_START		0x50	//
#define	CMD_EP6IN_STOP		0x51	//
#define	CMD_EP2OUT_START	0x52	//
#define	CMD_EP2OUT_STOP		0x53	//
#define	CMD_PORT_CFG		0x54	//addr_mask, out_pins
#define	CMD_REG_READ		0x55	//addr	(return 1byte)
#define	CMD_REG_WRITE		0x56	//addr, value
#define	CMD_PORT_READ		0x57	//(return 1byte)
#define	CMD_PORT_WRITE		0x58	//value
#define	CMD_IFCONFIG		0x59	//value
#define	CMD_MODE_IDLE		0x5a

#define BUF_LEN	(512*64)	//USB転送に使用するバッファサイズ
#define	QUE_NUM	(64)		//USB転送に使用するバッファの個数
#define PIECE_BUF_LEN	(512*8)	//PIECEのUSB転送に使用するバッファサイズ(小さくしないと遅延が大きくなる)
#define	PIECE_QUE_NUM	(64)	//USB転送に使用するバッファの個数(小さくしないと遅延が大きくなる)

#define PIO_DROP	0x20
#define	PIO_RESET	0x10
#define PIO_DIR		0x08

/*!
	@brief MAX2に渡すキャプチャモード定数
*/
enum ECAPFPS{
	ECAPFPS_60	= 0x00,
	ECAPFPS_30	= 0x01,
	ECAPFPS_20	= 0x02,
	ECAPFPS_15	= 0x03,
};

/*!
	@brief	偽トロからのデータ取得用クラス
*/
class CNISETRO{
private:
	// 関数用定義
	typedef bool (*nise_func)( u8* ,u32 , _cusb2_tcb* );
	
	cusb2			*usb;
	int				m_cusb2id;
	cusb2_tcb		*tcb1;			// スレッド

	LONG			cmd_len, ret_len;
	u8				cmd[64];
	u8				ret[64];
	CCyUSBEndPoint	*inep;
	CCyUSBEndPoint	*outep;

	bool			top_cut;			//先頭64Kbyteをカットするフラグ
	bool			usb_init;
	unsigned long	m_ulErrFrm;			//! エラーフレームのカウント
	double			m_dFps;				//! 画面データの更新数
	FILE*			pRecfile;
	bool			m_bDropFrame;		//! ドロップフラグ
	unsigned long	m_CapBufCur;		//! キャプチャー用カーソル
	CMUTEX			m_mutex;
	int				m_ccapsize;			// キャプチャーバッファサイズ
	int				m_scrsize;			//	描画用バッファサイズ
	BYTE			*p_CaptureBuffer;	//! キャプチャーバッファ(動的生成)
	BYTE			*p_ScrData;			//! 描画用バッファ(18Bit)(動的生成)
	int				m_capcounter;		//　キャプチャカウンタ(1フレ生成毎に++)
	
	/*!
		@brief	FPSを取得する
		@return FPSが double で返される
		@note	※毎フレーム呼び出す必要がある
	*/
	double checkFPS( void );
	
	static bool getdata_from_cusb2_func(u8 *buf, u32 len , _cusb2_tcb* tcb);
	
	void func_first( u8* buf , u32 len );
	
	//! パラメータの初期化
	void param_init( int scrsize, int ccapsize ){
		cmd_len=0;
		ret_len=0;
		top_cut = false;
		
		memset( &cmd[0] , 0 , sizeof(u8) * 64 );
		memset( &ret[0] , 0 , sizeof(u8) * 64 );
		
		m_ulErrFrm = 0;
		m_dFps = 0;
		
		m_CapBufCur = 0;
		m_scrsize = scrsize;
		m_ccapsize = ccapsize;
	}

	void create_buffer(int scrsize, int ccapsize){
		p_CaptureBuffer	= new BYTE[ccapsize];
		p_ScrData		= new BYTE[scrsize];
		memset( p_CaptureBuffer , 0 , sizeof(BYTE) * ccapsize );
		memset( p_ScrData , 0xFF , sizeof(BYTE) * scrsize );
	}

	void delete_buffer( void ){
		if(p_CaptureBuffer != NULL) delete p_CaptureBuffer;
		if(p_ScrData != NULL) delete p_ScrData;
	}
	
public:
	CNISETRO(int scrsize, int ccapsize){
		usb_init = false;
		usb = NULL;
		pRecfile = NULL;
		m_bDropFrame = false;
		m_mutex.create();
		m_capcounter = 0;
		create_buffer(scrsize, ccapsize);
		param_init(scrsize, ccapsize);
		//
	}
	
	~CNISETRO(){
		rec_stop();
		if( usb_init ){
			NisetroQuit();
		}
		delete_buffer();
		m_mutex.destroy();
	}

	void change_buffersize(int scrsize, int ccapsize){
		BYTE *swapcap = p_CaptureBuffer;
		BYTE *swapscr = p_ScrData;

		create_buffer(scrsize, ccapsize);
		if (swapcap != NULL) delete swapcap;
		if (swapscr != NULL) delete swapscr;
		m_scrsize  = scrsize;
		m_ccapsize = ccapsize;
	}

	
	/*
		@brief カメレオンUSB2 FX制御用クラスを取得する
	*/
	cusb2* GetCUSB2( void ){ return usb; }
	
	/*!
		@brief	初期化処理
		@param	eFps	[in] フレーム数
		@param	eScr	[in] 取得する画面
		@param	id		[in] カメレオンusbのID
		@param	hWnd	[in] メッセージを受け取るウィンドウハンドル
	*/
	int NisetroInit( ECAPFPS eFps , int id = 0 , HWND hWnd = NULL );
	
	//! 終了処理
	void NisetroQuit( void );
	
	/*!
		@brief	スクリーンデータを取得する
		@param	pScrData	[in] 受け取るバッファへのポインタ
		@param	BufSize		[in] 受け取るバッファサイズ
		@param	dwWaitMS	[in] 受け取るまでの制限時間(ms)
	
		@ref nise_screen_dataformat
	*/
	int GetScrData( BYTE* pScrData , size_t BufSize , DWORD dwWaitMS = INFINITE ){
		if( !(BufSize >= (size_t)m_scrsize && pScrData) )
			return -1;
		
		if( m_mutex.lock(dwWaitMS) == false ){
#if _MSC_VER >= 1400
			memcpy_s( pScrData , BufSize , p_ScrData , m_scrsize );
#else
			memcpy( pScrData , p_ScrData , m_scrsize );
#endif
			m_mutex.unlock();
		}else
			return -1;
		return 0;
	}
	
	//! エラーフレームの設定を取得
	bool GetDropFrame( void ){
		return m_bDropFrame;
	}
	
	//! エラーフレームを設定する
	bool SetDropFrame( bool bDropFrame ){
		return (m_bDropFrame = bDropFrame);
	}
	
	//! 取得したエラーフレームの数を取得する
	int GetErrorFrameCount( void ){ return m_ulErrFrm; }
	
	//! データ更新頻度を取得する
	double GetFps( void ){ return m_dFps; }

	int GetCapCounter( void ){	
		int ret = 0;
		if( this->m_mutex.lock(INFINITE) == false){
			ret =  m_capcounter;
			this->m_mutex.unlock();
		}
		return ret;
	}

	void DecCapCounter( void ) {
		if( this->m_mutex.lock(INFINITE) == false){
			if (m_capcounter > 0)
				m_capcounter--;
			this->m_mutex.unlock();
		}
	}
	
	/*!
		@brief	フレームデータの保存を開始する
		@param	filename	[in] 保存するファイル名
	*/
	int rec_start( TCHAR* filename ){
		if( FILENAME_MAX == NULL ) return -1;
		
#if _MSC_VER >= 1400
		errno_t e = _tfopen_s( &pRecfile , filename , TEXT("wb") );
		if( e != 0 )
			return -1;
#else
		pRecfile = _tfopen( filename , TEXT("wb") );
		if( pRecfile == NULL )
			return -1;
#endif
		
		return 0;
	}
	
	//! フレームデータの保存を中止する
	void rec_stop( void ){
		if( pRecfile ){
			fclose( pRecfile );
			pRecfile = NULL;
		}
	}
	
	//! フレーム保存状態を取得する
	bool IsRec( void ){
		return (this->pRecfile != NULL);
	}
	
	bool IsInit( void ){ return usb_init; }
	
	//
};
#endif

