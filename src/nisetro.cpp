
/*!
	@file	nisetro.cpp
	@brief	�U�g���L���v�`������p
	//
*/
#include <time.h>
#include "nisetro.h"

#include "nisetro_if_conv.h"
#include "gameconsole.h"
extern CCONSOLEGAMES g_console;

extern "C" {
#include "_tprintd.h"
};

#ifdef DEBUG_FW
u8 fw_iic[1024*8];
#else
u8 fw_iic[]=
#include "fw.inc"
#endif
	/*!
	@brief	FPS���擾����
	@return FPS�� double �ŕԂ����
	@note	�����t���[���Ăяo���K�v������
*/

//data = 128*88
//conv = 128*88*3
void piece_conv(BYTE* conv, const BYTE* data)
{
	int i,j,k,l;
	int num;

	l = 0;
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 88; j++) {
			for (k = 0; k < 8; k++) {
				//8 * 88 ��]���㎟�̉�8���C���̃f�[�^�ƂȂ�
				//�����128*88�ƂȂ�悤�ɏ��Ԃ����ւ���
				//�܂�RGB�e1�o�C�g�Ƃ��Ȃ���΂Ȃ�Ȃ����ߓ����f�[�^��3�o�C�g���i�[����
				num = (i*8 + j*128 + k) * 3;
				conv[num] = conv[num+1] = conv[num+2] = data[l++]; //R
			}
		}
	}
	return ;
}

double CNISETRO::checkFPS( void )
{
	static time_t	last = 0;
	static DWORD	frames = 0;
	static double	fps = 0;
	time_t			current;
	
	time( &current );
	frames++;
	
	if( current != last) {// 1�b���ɍX�V
		double dt = (double)(current - last);
		fps = (double)frames / dt;
		last = current;
		frames = 0;
	}
	return fps;
}

void CNISETRO::func_first( u8* buf , u32 len ){
	CNISETRO* pNise = this;
	
	for( u32 i = 0; i < len; i++ ){
		u32 cur = 0;
		u8 d = buf[i];
		bool bVSync = ((d & 0x80) == 0x80);
		
		// �Ȃ񂩔���
		if( (30*3) < pNise->m_CapBufCur && bVSync ){
			bool bError = false;
			
			// �G���[�`�F�b�N
			unsigned long ulScrBufSize = pNise->m_scrsize;
			if( pNise->m_CapBufCur < ulScrBufSize ){
				if( pNise->m_ulErrFrm++ == 0xFFFFFFFF )	pNise->m_ulErrFrm = 0;
				bError = true;
			}
			
			// �X�N���[���f�[�^�Ɉڂ�
			if( !(pNise->m_bDropFrame && bError) ){
				unsigned char lSel = 0;
				if( pNise->m_mutex.lock(INFINITE) == false){
					m_dFps = checkFPS();
					
					size_t write_size = pNise->m_scrsize;
					if(g_console.GetConsole() == CONSOLE_PIECE){
						piece_conv(pNise->p_ScrData, pNise->p_CaptureBuffer);
					} else {
#if _MSC_VER >= 1400
						errno_t e = memcpy_s( pNise->p_ScrData, pNise->m_scrsize , pNise->p_CaptureBuffer , write_size );
#else
						memcpy( pNise->p_ScrData, pNise->p_CaptureBuffer , write_size );
#endif
					}
					m_capcounter++;
					pNise->m_mutex.unlock();
				}
			}
			
			pNise->m_CapBufCur = 0;
		}
		
		cur = pNise->m_CapBufCur++;
		
		pNise->p_CaptureBuffer[cur] = d;
		
		// �o�b�t�@�[�I�[�o�[���C�g��h�����߂�
		if( pNise->m_CapBufCur > (unsigned)pNise->m_scrsize )			
			pNise->m_CapBufCur = 0;
	}
}

//! USB�f�[�^��M���ɌĂ΂��֐�
bool CNISETRO::getdata_from_cusb2_func(u8 *buf, u32 len , _cusb2_tcb* tcb){
	CNISETRO* pNise = NULL;
	
	pNise = (CNISETRO*)tcb->userpointer;
	if( pNise == NULL ) return true;

	if( pNise->top_cut ){
		pNise->top_cut = false;
		return true;
	}
	
	if( pNise->IsRec() ){
		fwrite( buf , sizeof(u8) , len , pNise->pRecfile );
	}
	
	pNise->func_first( buf , len );	
	
	//�I�������̃`�F�b�N
	if( tcb->looping == true ){
		// �X���b�h���I��点��ꍇ�� false
		return true;
	}

	// �I������
	return false;
}

/*
	@brief	����������
	@param	eFps	[in] �t���[����
	@param	eScr	[in] �擾������
	@param	id		[in] �J�����I��usb��ID
	@param	hWnd	[in] �E�B���h�E�n���h��
*/
int CNISETRO::NisetroInit( ECAPFPS eFps , int id , HWND hWnd ){
	if( usb_init )
		return -1;
	
	usb = new cusb2( hWnd );
	if( usb == NULL )
		return -1;
	
	m_cusb2id = id;
	
	if( usb->fwload(m_cusb2id/* id */, fw_iic,(u8 *)"CFX2LOG") == false ){
		_tprintd( TEXT("error nisetro fwload\n") );
		return -1;
	}
	
	inep = usb->get_endpoint(0x81);
	outep = usb->get_endpoint(0x01);
	
	memset( &cmd[0] , 0 , sizeof(u8) * 64 );
	memset( &ret[0] , 0 , sizeof(u8) * 64 );

	//FX2 I/O�|�[�g�ݒ�@���@MAX2�����p�����[�^�ݒ� �� MAX2���Z�b�g
	cmd_len=0;
	cmd[cmd_len++]=CMD_PORT_CFG;
	cmd[cmd_len++]=0x07;					//ADDR[2:0]
	cmd[cmd_len++]=PIO_RESET | PIO_DIR;		//RESET,DIR(out_pins)
	cmd[cmd_len++]=CMD_MODE_IDLE;
	cmd[cmd_len++]=CMD_IFCONFIG;
	cmd[cmd_len++]=0xE3;					//slave FIFO, IFCLK=48MHz, IFCLK�o��,  ����(Sync)���[�h

	cmd[cmd_len++]=CMD_REG_WRITE;
	cmd[cmd_len++]=0;						//ADDR=0
	cmd[cmd_len] = 0;
	cmd[cmd_len] |= (u8)eFps;				// frame rate
	cmd_len++;
	cmd[cmd_len++]=CMD_MODE_IDLE;
	usb->xfer(outep, cmd, cmd_len);
	cmd_len=0;

	// �X���b�h�̊J�n
	if ( g_console.GetConsole() == CONSOLE_PIECE )
		tcb1 = usb->start_thread(0x86, PIECE_BUF_LEN, PIECE_QUE_NUM, this->getdata_from_cusb2_func );
	else
		tcb1 = usb->start_thread(0x86, BUF_LEN, QUE_NUM, this->getdata_from_cusb2_func );
	tcb1->userpointer = (void*)this;

	cmd_len=0;
	cmd[cmd_len++]=CMD_PORT_WRITE;
	cmd[cmd_len++]=PIO_RESET;
	cmd[cmd_len++]=CMD_EP6IN_START;
	cmd[cmd_len++]=CMD_PORT_WRITE;
	cmd[cmd_len++]=0;
	usb->xfer(outep, cmd, cmd_len);
	cmd_len=0;
	usb_init = true;
	return 0;
}

// �I������
void CNISETRO::NisetroQuit( void ){
	if( usb_init == false ){
		if( usb ){
			delete usb;
			usb = NULL;
		}
		param_init(m_scrsize, m_ccapsize);
		return;
	}
	
	tcb1->looping = false;
	usb->delete_thread(tcb1);
	
	cmd_len=0;
	cmd[cmd_len++]=CMD_PORT_WRITE;
	cmd[cmd_len++]=0;
	cmd[cmd_len++]=CMD_EP6IN_STOP;
	cmd[cmd_len++]=CMD_MODE_IDLE;
	usb->xfer(outep, cmd, cmd_len);
	cmd_len=0;
	
	cmd_len=0;
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x00;			//all_cnt[7:0]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x01;			//all_cnt[15:8]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x02;			//all_cnt[23:16]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x03;			//all_cnt[31:24]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x04;			//err_cnt[7:0]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x05;			//err_cnt[15:8]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x06;			//err_cnt[23:16]
	cmd[cmd_len++]=CMD_REG_READ;
	cmd[cmd_len++]=0x07;			//err_cnt[31:24]
	if( outep )usb->xfer(outep, cmd, cmd_len);
	cmd_len=0;
	ret_len=8;
	inep = usb->get_endpoint(0x81);	// �A�h���X���ς���Ă��邱�Ƃ��������̂Ŏ擾����悤�ɂ���
	if( inep )usb->xfer(inep, ret, ret_len);
	
	delete usb;
	usb = NULL;
	
	usb_init = false;
	param_init(m_scrsize, m_ccapsize);
}


