
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "cmutex.h"

#ifndef __CWAVESAVE_H__
#define __CWAVESAVE_H__

//! 録音用バッファの数　2以上推奨
const int _CWS_BUFFERCOUNT = 2;
const int _CWS_BUFFERTIME = 1;	// sec

/*!
	@brief	WaveInによる簡易録音クラス
*/
class CWAVEINSAVE {
protected:
	//! 初期化フラグ
	bool			m_bInit;
	
	//! 録音フラグ
	bool			m_bRecord;
	
	//! ミューテクスクラス
	CMUTEX			m_Mutex;
	
	//! ファイルポインタ
	FILE*			m_pOut;
	
	//! デバイスハンドル
	HWAVEIN			m_hWavein;
	
	//! フォーマット
	WAVEFORMATEX	m_WFmt;
	
	//! ハンドル
	WAVEHDR			m_WHdr[_CWS_BUFFERCOUNT];
	
	/*!
		@brief	ファイルへヘッダを書き込む
		@param	fp		[in] 書き込むファイルへのポインタ
		@param	pWFmt	[in] waveフォーマットへのポインタ
	*/
	static void writeWaveHeader( FILE* fp , LPWAVEFORMATEX pWFmt ){
		if( !(fp && pWFmt) ) return;
		
		fseek( fp , 0 , SEEK_END );
		long lSize = ftell(fp);
		
		long t = 0;
		fseek( fp , 0 , SEEK_SET );
		fwrite( (void*)"RIFF" , sizeof(char) , 4 , fp );
		t = lSize;
		fwrite( (void*)&t , sizeof(long) , 1 , fp );
		fwrite( (void*)"WAVE" , sizeof(char) , 4 , fp );
		
		fwrite( (void*)"fmt " , sizeof(char) , 4 , fp );
		t = sizeof(WAVEFORMATEX)-2;
		fwrite( (void*)&t , sizeof(long) , 1 , fp );
		fwrite( (void*)pWFmt , sizeof(WAVEFORMATEX)-2 , 1 , fp );
		
		fwrite( (void*)"data" , sizeof(char) , 4 , fp );
		t = lSize - 0x2C;
		fwrite( (void*)&t , sizeof(long) , 1 , fp );
		fseek( fp , 0 , SEEK_END );
	}

public:
	CWAVEINSAVE(){
		m_bInit = false;
		m_bRecord = false;
		m_pOut = NULL;
		memset( &m_WFmt , 0 , sizeof(WAVEFORMATEX) );
		memset( m_WHdr , 0 , sizeof(WAVEHDR) * _CWS_BUFFERCOUNT );
		m_Mutex.create();
	}
	
	~CWAVEINSAVE(){
		this->Stop();
		this->Quit();
		m_Mutex.destroy();
	}
	
	/*!
		@brief	保存用コールバック
	*/
	static int CALLBACK _waveinCallback( HWAVEIN hwi , UINT uMsg , DWORD_PTR dwInstance , DWORD_PTR dwParam1 , DWORD_PTR dwParam2 ){
		CWAVEINSAVE* pThis = (CWAVEINSAVE*)dwInstance;
		switch( uMsg ){
			case WIM_OPEN:
				{
					_tprintf( TEXT("WIM_OPEN\n") );
				}
				break;
			
			case WIM_CLOSE:
				{
					int rc = 0;
					_tprintf( TEXT("WIM_CLOSE\n") );
				}
				break;
			
			case WIM_DATA:
				{
					int rc = 0;
					LPWAVEHDR pWHdr = (LPWAVEHDR) dwParam1;
					if( pThis->m_bInit == false ) break;
					
					_tprintf( TEXT("WIM_DATA\n") );
					if( (pWHdr->dwFlags & WHDR_DONE) != WHDR_DONE )
						break;
					
					if( pThis->m_Mutex.lock(INFINITE) == false ){
						if( pWHdr->lpData && pThis->m_pOut )
							fwrite( pWHdr->lpData , sizeof(char) , pWHdr->dwBytesRecorded , pThis->m_pOut );
						pThis->m_Mutex.unlock();
					}
					
					if( pThis->m_bRecord ){
						_tprintf( TEXT("waveInUnprepareHeader\n") );
						rc = waveInUnprepareHeader(hwi, pWHdr , sizeof(WAVEHDR));
						
						pWHdr->dwFlags = 0;
						
						_tprintf( TEXT("waveInPrepareHeader\n") );
						rc = waveInPrepareHeader(hwi, pWHdr, sizeof(WAVEHDR));
						
						_tprintf( TEXT("waveInAddBuffer\n") );
						rc = waveInAddBuffer(hwi, pWHdr, sizeof(WAVEHDR));
					}
				}
				break;
			
			default:
				_tprintf( TEXT("default\n") );
				break;
		}
		
		return 0;
	};
	
	/*!
		@brief	初期化処理
		@param	ID				[in] WaveIn用デバイスＩＤ
		@param	iChannels		[in] チャンネル数
		@param	iSamplesPerSec	[in] 周波数
		@param	iBps			[in] ビット数(1Sampleあたり)
	*/
	int Init( UINT ID = WAVE_MAPPER , int iChannels = 2 , int iSamplesPerSec = 44100 , int iBps = 16 ){
		int rc = 0;
		if( m_bInit ) return 0;
		
		_tprintf( TEXT("Init\n") );
		memset( &m_WFmt , 0 , sizeof(WAVEFORMATEX) );
		m_WFmt.wFormatTag		= WAVE_FORMAT_PCM;
		m_WFmt.nChannels		= iChannels;
		m_WFmt.nSamplesPerSec	= iSamplesPerSec;
		m_WFmt.wBitsPerSample	= iBps;
		m_WFmt.nBlockAlign		= m_WFmt.nChannels * m_WFmt.wBitsPerSample / 8;
		m_WFmt.nAvgBytesPerSec	= m_WFmt.nSamplesPerSec * m_WFmt.nBlockAlign;
		
		int blocksize = m_WFmt.nSamplesPerSec * m_WFmt.nBlockAlign * _CWS_BUFFERTIME;
		
		_tprintf( TEXT("waveInOpen\n") );
		rc = waveInOpen( &m_hWavein ,ID, &m_WFmt , (DWORD_PTR)_waveinCallback , (DWORD_PTR)this , CALLBACK_FUNCTION );
		if(rc!= MMSYSERR_NOERROR){
			TCHAR msg[255];
			waveInGetErrorText(rc,msg,sizeof(msg));
			_tprintf( TEXT("Errer %s\n") , msg );
			return rc;
		}
		
		for( int i = 0; i < _CWS_BUFFERCOUNT; i++ ){
			memset( &m_WHdr[i] , 0 , sizeof(WAVEHDR) );
			m_WHdr[i].lpData = new char[blocksize];
			m_WHdr[i].dwBufferLength = blocksize;
			_tprintf( TEXT("waveInPrepareHeader\n") );
			rc = waveInPrepareHeader( m_hWavein,&m_WHdr[i],sizeof(WAVEHDR));
			if(rc!= MMSYSERR_NOERROR){
				TCHAR msg[255];
				waveInGetErrorText(rc,msg,sizeof(msg));
				_tprintf( TEXT("Errer %s\n") , msg );
				return rc;
			}
			
			_tprintf( TEXT("waveInAddBuffer\n") );
			rc = waveInAddBuffer(m_hWavein,&m_WHdr[i],sizeof(WAVEHDR));
			if(rc!= MMSYSERR_NOERROR){
				TCHAR msg[255];
				waveInGetErrorText(rc,msg,sizeof(msg));
				_tprintf( TEXT("Errer %s\n") , msg );
				return rc;
			}
		}
		
		m_bInit = true;
		
		return 0;
	}
	
	//! 終了処理
	void Quit( void ){
		int rc = 0;
		if( m_bInit == false )
			return;
		
		_tprintf( TEXT("Quit\n") );
		
		_tprintf( TEXT("waveInReset\n") );
		rc = waveInReset(m_hWavein);
		if(rc!= MMSYSERR_NOERROR){
			TCHAR msg[255];
			waveInGetErrorText(rc,msg,sizeof(msg));
			_tprintf( TEXT("Errer %s\n") , msg );
		}
		
		for( int i = 0; i < _CWS_BUFFERCOUNT; i++ ){
			if( m_WHdr[i].lpData ){
				_tprintf( TEXT("waveInUnprepareHeader\n") );
				rc = waveInUnprepareHeader( m_hWavein,&m_WHdr[i],sizeof(WAVEHDR));
				if(rc!= MMSYSERR_NOERROR){
					TCHAR msg[255];
					waveInGetErrorText(rc,msg,255);
					_tprintf( TEXT("Errer %s\n") , msg );
				}else{
					delete [] m_WHdr[i].lpData;
					m_WHdr[i].lpData = NULL;
				
				}
			}
		}
		
		if( m_hWavein ){
			_tprintf( TEXT("waveInClose\n") );
			rc = waveInClose( m_hWavein );
			m_hWavein = NULL;
			if(rc!= MMSYSERR_NOERROR){
				TCHAR msg[255];
				waveInGetErrorText(rc,msg,sizeof(msg));
				_tprintf( TEXT("Errer %s\n") , msg );
			}
		}
		m_bInit = false;
	}
	
	//! 録音の開始
	int Start( TCHAR* pFilename ){
		int rc = 0;
		if( m_bInit == false ) return -1;
		
#if _MSC_VER >= 1400
		_tfopen_s( &m_pOut , pFilename , TEXT("wb") );
#else
		m_pOut = _tfopen( pFilename , TEXT("wb") );
#endif
		if( m_Mutex.lock(INFINITE) == false ){
			writeWaveHeader( m_pOut , &m_WFmt );
			m_Mutex.unlock();
		}
		
		_tprintf( TEXT("waveInStart\n") );
		rc = waveInStart(m_hWavein);
		if(rc!= MMSYSERR_NOERROR){
			TCHAR msg[255];
			waveInGetErrorText(rc,msg,sizeof(msg));
			_tprintf( TEXT("Errer %s\n") , msg );
			return rc;
		}
		
		m_bRecord = true;
		
		return 0;
	}
	
	//! 録音の終了
	int Stop( void ){
		int rc = 0;
		if(m_pOut == NULL)
			return -1;
		
		_tprintf( TEXT("waveInStop\n") );
		rc = waveInStop(m_hWavein);
		if(rc!= MMSYSERR_NOERROR){
			TCHAR msg[255];
			waveInGetErrorText(rc,msg,sizeof(msg));
			_tprintf( TEXT("Errer %s\n") , msg );
			return rc;
		}
		
		Sleep(1000*_CWS_BUFFERTIME);
		
		m_bRecord = false;
		
		writeWaveHeader( m_pOut , &m_WFmt );
		fclose( m_pOut );
		m_pOut = NULL;
		
		
		
		return 0;
	}
	//
};

#endif

