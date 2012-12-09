#include <stdio.h>
#include <tchar.h>
#include <windows.h>

#include "cusb2.h"
#include "cmutex.h"

#ifndef __NISETROINFO_H__
#define __NISETROINFO_H__
// CUSB2 �ɑ���R�}���h�炵��
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

#define BUF_LEN	(512*64)	//USB�]���Ɏg�p����o�b�t�@�T�C�Y
#define	QUE_NUM	(64)		//USB�]���Ɏg�p����o�b�t�@�̌�
#define PIECE_BUF_LEN	(512*8)	//PIECE��USB�]���Ɏg�p����o�b�t�@�T�C�Y(���������Ȃ��ƒx�����傫���Ȃ�)
#define	PIECE_QUE_NUM	(64)	//USB�]���Ɏg�p����o�b�t�@�̌�(���������Ȃ��ƒx�����傫���Ȃ�)

#define PIO_DROP	0x20
#define	PIO_RESET	0x10
#define PIO_DIR		0x08

/*!
	@brief MAX2�ɓn���L���v�`�����[�h�萔
*/
enum ECAPFPS{
	ECAPFPS_60	= 0x00,
	ECAPFPS_30	= 0x01,
	ECAPFPS_20	= 0x02,
	ECAPFPS_15	= 0x03,
};

/*!
	@brief	�U�g������̃f�[�^�擾�p�N���X
*/
class CNISETRO{
private:
	// �֐��p��`
	typedef bool (*nise_func)( u8* ,u32 , _cusb2_tcb* );
	
	cusb2			*usb;
	int				m_cusb2id;
	cusb2_tcb		*tcb1;			// �X���b�h

	LONG			cmd_len, ret_len;
	u8				cmd[64];
	u8				ret[64];
	CCyUSBEndPoint	*inep;
	CCyUSBEndPoint	*outep;

	bool			top_cut;			//�擪64Kbyte���J�b�g����t���O
	bool			usb_init;
	unsigned long	m_ulErrFrm;			//! �G���[�t���[���̃J�E���g
	double			m_dFps;				//! ��ʃf�[�^�̍X�V��
	FILE*			pRecfile;
	bool			m_bDropFrame;		//! �h���b�v�t���O
	unsigned long	m_CapBufCur;		//! �L���v�`���[�p�J�[�\��
	CMUTEX			m_mutex;
	int				m_ccapsize;			// �L���v�`���[�o�b�t�@�T�C�Y
	int				m_scrsize;			//	�`��p�o�b�t�@�T�C�Y
	BYTE			*p_CaptureBuffer;	//! �L���v�`���[�o�b�t�@(���I����)
	BYTE			*p_ScrData;			//! �`��p�o�b�t�@(18Bit)(���I����)
	int				m_capcounter;		//�@�L���v�`���J�E���^(1�t����������++)
	
	/*!
		@brief	FPS���擾����
		@return FPS�� double �ŕԂ����
		@note	�����t���[���Ăяo���K�v������
	*/
	double checkFPS( void );
	
	static bool getdata_from_cusb2_func(u8 *buf, u32 len , _cusb2_tcb* tcb);
	
	void func_first( u8* buf , u32 len );
	
	//! �p�����[�^�̏�����
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
		@brief �J�����I��USB2 FX����p�N���X���擾����
	*/
	cusb2* GetCUSB2( void ){ return usb; }
	
	/*!
		@brief	����������
		@param	eFps	[in] �t���[����
		@param	eScr	[in] �擾������
		@param	id		[in] �J�����I��usb��ID
		@param	hWnd	[in] ���b�Z�[�W���󂯎��E�B���h�E�n���h��
	*/
	int NisetroInit( ECAPFPS eFps , int id = 0 , HWND hWnd = NULL );
	
	//! �I������
	void NisetroQuit( void );
	
	/*!
		@brief	�X�N���[���f�[�^���擾����
		@param	pScrData	[in] �󂯎��o�b�t�@�ւ̃|�C���^
		@param	BufSize		[in] �󂯎��o�b�t�@�T�C�Y
		@param	dwWaitMS	[in] �󂯎��܂ł̐�������(ms)
	
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
	
	//! �G���[�t���[���̐ݒ���擾
	bool GetDropFrame( void ){
		return m_bDropFrame;
	}
	
	//! �G���[�t���[����ݒ肷��
	bool SetDropFrame( bool bDropFrame ){
		return (m_bDropFrame = bDropFrame);
	}
	
	//! �擾�����G���[�t���[���̐����擾����
	int GetErrorFrameCount( void ){ return m_ulErrFrm; }
	
	//! �f�[�^�X�V�p�x���擾����
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
		@brief	�t���[���f�[�^�̕ۑ����J�n����
		@param	filename	[in] �ۑ�����t�@�C����
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
	
	//! �t���[���f�[�^�̕ۑ��𒆎~����
	void rec_stop( void ){
		if( pRecfile ){
			fclose( pRecfile );
			pRecfile = NULL;
		}
	}
	
	//! �t���[���ۑ���Ԃ��擾����
	bool IsRec( void ){
		return (this->pRecfile != NULL);
	}
	
	bool IsInit( void ){ return usb_init; }
	
	//
};
#endif

