
#include "nisetro.h"

#ifndef __CONFIGIO_H__
#define __CONFIGIO_H__

// ��ʂ̌����Ɋւ���萔
#define DIRMODE_NORMAL		0		// �ʏ�
#define DIRMODE_LEFT		1		// ��90�x��]
#define DIRMODE_RIGHT		2		// �E90�x��]
#define DIRMODE_KERORICAN	3		// �P�����J���p��]
#define DIRMODE_MAX			4		// ���ڐ�

// �P�����J���̉�]�p�x
#define DEGREE_KERORICAN	-26.5f	// ��30�x��]

//! �g�嗦�̕ϓ��̊���
const float CSCRSCAL_SCALE = 10.0f;	// 10%����(�R)

//! �g�嗦�̍ŏ��l
const float CSCRSCAL_MIN = 0.5f;

//! �g�嗦�̍ő�l
const float CSCRSCAL_MAX = 5.0f;

//! �ݒ�����i�[����^
typedef struct SAPPCONFIG {
	//! ��ʂ̌���
	unsigned char	m_DirMode;

	//! �g�嗦
	float			m_fScrScal;
	
	//! �\���t���O(��O�ɕ\������)
	bool			m_bTopWindow;

	//! �G���[���������ꍇ�A�\�����X�V���Ȃ�
	bool			m_bDropFrame;
	
	// �J�����I��USB�̂h�c
	int				m_iCUSB2_ID;
	
	//! �\�����鑬�x
	ECAPFPS			m_eFrmSkip;
	
	//! �g�p�n�[�h�E�F�A
	int				m_iconsole;
} SAPPCONFIG , *PAPPCONFIG , *LPAPPCONFIG;

/*!
	@brief	�ݒ肩��N���C�A���g�T�C�Y���擾����
	@param	pAppConfig	[in] �ݒ�ϐ��ւ̃|�C���^
	@param	pRC			[out] �N���C�A���g�T�C�Y
*/
void GetClientSizeForAppconfig( LPAPPCONFIG pAppConfig , RECT *pRC );

/*!
	@brief	�f�o�b�N�\���p�֐�
	@param	pAppConfig	[in] �\������ݒ�ϐ��ւ̃|�C���^
	@note	�\�����@��_printd�Ɉˑ�����B
	
*/
void ShowAppConfig( LPAPPCONFIG pAppConfig );

/*!
	@brief	�ݒ���t�@�C��(xml)�ɏ����o��
	@param	pConfig		[in] �ݒ�ϐ��ւ̃|�C���^
	@param	pFilename	[in] �t�@�C����
*/
int ConfigIOSave( PAPPCONFIG pConfig , wchar_t* pFilename );

/*!
	@brief	�ݒ�t�@�C��(xml)����ݒ��ǂ�
	@param	pConfig		[out] �ݒ�ϐ��ւ̃|�C���^
	@param	pFilename	[in] �t�@�C����
*/
int ConfigIOLoad( PAPPCONFIG pConfig , wchar_t* pFilename );

/*!
	@brief	WinMain����̃R�}���h���C������ݒ�����擾����B
	@param	pConfig		[out] �ݒ�ϐ��ւ̃|�C���^
	@param	lpszCmdLine	[in] �R�}���h���C��������
*/
int ConfigIOCheckCmdLine( PAPPCONFIG pConfig , LPTSTR lpszCmdLine );

#endif

