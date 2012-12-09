/*
blur.h

Copyright (C) 2012-2012 gbm

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef __BLUR_H__
#define __BLUR_H__

#include <Windows.h>
#include <d3dx9.h>

#include "cmutex.h"
extern "C" {
#include "_tprintd.h"
};

#define BLUR_TEXTURE_MAX	10
#define BLUR_TYPE_MAX		8

#define BLUR_TYPE_NONE		0
#define	BLUR_TYPE_GBCMODE	1
#define BLUR_TYPE_GBMODE	2
#define BLUR_TYPE_GBPMODE	3
#define BLUR_TYPE_WSCMODE	4
#define BLUR_TYPE_BLUR01	5
#define BLUR_TYPE_BLUR02	6
#define BLUR_TYPE_BLUR03	7

#define TEXTURE_WIDTH		256
#define	TEXTURE_HEIGHT		256

#define SAFE_RELEASE(x) { if(x){(x)->Release(); (x) = NULL;} }

class TEXTURE_BLUR
{
private:
	LPDIRECT3DTEXTURE9 m_pTexBlur[BLUR_TEXTURE_MAX];
	int m_blur_type;
	CMUTEX m_mutex;

	void rotate_texture(void);

public:
	TEXTURE_BLUR() {
		m_blur_type = BLUR_TYPE_NONE;
		m_mutex.create();
	}

	~TEXTURE_BLUR() {
		ReleaseTexture();
		m_mutex.destroy();
	}

	HRESULT	CreateTexture( LPDIRECT3DDEVICE9 *pD3DDev );
	void	ReleaseTexture(void);
	HRESULT	SetTexture(const BYTE* p_ScrData);
	HRESULT	ClearTexture(void);

	void	RenderBlur(RECT *SrcRc, D3DXVECTOR3 *vecCenter, D3DXVECTOR3 *vecPos, int type);

	int		GetBlurType(void) { return m_blur_type; }
	void	SetBlurType(int type) {
		if ( type >= BLUR_TYPE_MAX ) return;
		m_blur_type = type;
	}
};

#endif