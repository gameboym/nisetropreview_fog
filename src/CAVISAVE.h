
extern "C" { // これ重要
	//#define inline _inline
	#include <libavutil/avutil.h>
	#include <libavcodec/avcodec.h>
	#include <libavcodec/audioconvert.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/fifo.h>

	#include "_tprintd.h"
}

#include "cthread.h"
#include "cmutex.h"

#ifndef __CAVISAVE_H__
#define __CAVISAVE_H__


typedef struct _ffmpeg_struct{
	AVOutputFormat*		m_pFmt;
	AVFormatContext*	m_pOutFmtContext;
	AVStream*			m_pVStream;
	AVStream*			m_pAStream;
    
	struct {
		uint8_t*			m_pOutBuf;
		uint32_t			m_OutBufSize;
		AVFrame*			m_pPicture;
		AVFrame*			m_pInputPicture;
		struct SwsContext *m_img_convert_ctx;
	} m_Video;
	
	struct m_audio {
		AVCodecContext	*m_pDCodecCtx;
		AVCodecContext	*m_pECodecCtx;
		
		AVFifoBuffer *m_fifo;
		ReSampleContext *m_rs;
		AVPacket pkt;
		
		struct {
			uint8_t *data_buf;
			uint8_t *audio_out;
			uint8_t *buffer_resample;
		} m_tmpbuf;
	} m_Audio;
	
}_ffmpeg_struct;

int ffmpeginit( _ffmpeg_struct* pffm , char* pFilename , 
			   CodecID vcodec_id , 
			   long width , long height , double fps = 15 ,  int bitrate_scale = 64, 
			   CodecID acodec_id = CODEC_ID_NONE , 
			   int sample_rate = 44100 , int channels = 2 , int audio_bit_rate = 64000 );

void ffmpegquit( _ffmpeg_struct* pffm );

int ffmpeg_addVFrame( _ffmpeg_struct* pffm , unsigned char *pBuf , long width , long height );

int ffmpeg_addAFrame( _ffmpeg_struct* pffm , unsigned char *pBuf , size_t bufsize );

//! ビデオストリームを追加する
AVStream *ffmpeg_addVStream( AVFormatContext *oc,
		                                     CodecID codec_id ,
											 int w, int h, int bitrate,
											 double fps, int pixel_format);

//! オーディオストリームを追加する
AVStream* ffmpeg_addAStream(AVFormatContext *oc, int codec_id, int sample_rate = 44100 , int channels = 2 , int bit_rate = 64000 );

const int _CWS_BUFFERCOUNT = 2;
const int _CWS_BUFFERTIME = 1;	// sec

class CAVISAVECORE : public CTHREAD {
protected:
	bool			m_bInit;
	bool			m_bRecord;
	CMUTEX			m_Mutex;
	
	DWORD			m_dwWaitTime_Max;
	HWAVEIN			m_hWavein;
	WAVEFORMATEX	m_WFmt;
	WAVEHDR			m_WHdr[_CWS_BUFFERCOUNT];
	
	_ffmpeg_struct	m_ffm;
	
	static int CALLBACK _waveinCallback( HWAVEIN hwi , UINT uMsg , DWORD_PTR dwInstance , DWORD_PTR dwParam1 , DWORD_PTR dwParam2 ){
		CAVISAVECORE* pThis = (CAVISAVECORE*)dwInstance;
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
						if( pWHdr->lpData ){
						// write data
							if( ffmpeg_addAFrame( &pThis->m_ffm , (unsigned char*)pWHdr->lpData , pWHdr->dwBytesRecorded ) ){
								_tprintd( TEXT("ffmpeg_addAFrame\n") );
							}
						}
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
	}
	//
public:
	
	CAVISAVECORE(){
		m_bInit = false;
		m_bRecord = false;
		memset( &m_WFmt , 0 , sizeof(WAVEFORMATEX) );
		memset( m_WHdr , 0 , sizeof(WAVEHDR) * _CWS_BUFFERCOUNT );
		m_Mutex.create();
	}
	
	~CAVISAVECORE(){
		this->Stop();
		this->Quit();
		m_Mutex.destroy();
	}
	
	_ffmpeg_struct* GetFFMpeg( void ){ return &m_ffm; }
	
	bool IsRec( void ){ return m_bRecord; }
	bool IsInit( void ){ return m_bInit; }
	
	int Init( TCHAR* pFilename , CodecID vcodec_id , int width , int height , double fps = 15 , int iVBitRate = 6 * 1024 * 1024 , UINT ID = WAVE_MAPPER , CodecID acodec_id = CODEC_ID_NONE ,int iChannels = 2 , int iSamplesPerSec = 44100 , int iABitRate = 64000 ){
		int rc = 0;
		if( m_bInit ) return 0;
		
		_tprintf( TEXT("Init\n") );
		memset( &m_WFmt , 0 , sizeof(WAVEFORMATEX) );
		m_WFmt.wFormatTag		= WAVE_FORMAT_PCM;
		m_WFmt.nChannels		= iChannels;
		m_WFmt.nSamplesPerSec	= iSamplesPerSec;
		m_WFmt.wBitsPerSample	= 16;
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
		
		char strFilename[ MAX_PATH ];
	
#if defined(_UNICODE)
	#if _MSC_VER >= 1400
		wcstombs_s( NULL , strFilename , MAX_PATH , pFilename , MAX_PATH );
	#else
		wcstombs( strFilename , pFilename , sizeof(char) * MAX_PATH );
	#endif
#else
	#if _MSC_VER >= 1400
		strcpy_s( strFilename , MAX_PATH , pFilename );
	#else
		strcpy( strFilename , pFilename );
	#endif
#endif
		if( ffmpeginit( &m_ffm , strFilename , vcodec_id , width , height , fps , iVBitRate , acodec_id , m_WFmt.nSamplesPerSec , m_WFmt.nChannels , iABitRate )){
			_tprintd( TEXT("ffmpeginit\n") );
			return -1;
		}
	
		m_dwWaitTime_Max = (int)(1000 / fps);
		this->setSleepTime( m_dwWaitTime_Max );
		
		m_bInit = true;
		
		return 0;
	}
	
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
		
		ffmpegquit(  &m_ffm  );
		
		m_bInit = false;
	}
	
	int Start( void ){
		int rc = 0;
		if( m_bInit == false ) return -1;
		
		_tprintf( TEXT("Start\n") );
		
		_tprintf( TEXT("waveInStart\n") );
		rc = waveInStart(m_hWavein);
		if(rc!= MMSYSERR_NOERROR){
			TCHAR msg[255];
			waveInGetErrorText(rc,msg,sizeof(msg));
			_tprintf( TEXT("Errer %s\n") , msg );
			return rc;
		}
		
		this->startThread();
		m_bRecord = true;
		
		return 0;
	}
	
	int Stop( void ){
		int rc = 0;
		
		if( m_bRecord == false )
			return -1;
		
		_tprintf( TEXT("Stop\n") );
		
		_tprintf( TEXT("waveInStop\n") );
		rc = waveInStop(m_hWavein);
		if(rc!= MMSYSERR_NOERROR){
			TCHAR msg[255];
			waveInGetErrorText(rc,msg,sizeof(msg));
			_tprintf( TEXT("Errer %s\n") , msg );
			return rc;
		}
		
		this->stopThread();
		Sleep(1000*_CWS_BUFFERTIME*2);
		
		
		m_bRecord = false;
		return 0;
	}
	//
};

#endif

