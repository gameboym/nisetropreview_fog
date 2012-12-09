/*
blur.cpp

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


#include "blur.h"

HRESULT BufToTex(const BYTE *p_ScrData, LPDIRECT3DTEXTURE9 pTex);
HRESULT ClrTex(LPDIRECT3DTEXTURE9 pTex);

extern LPD3DXSPRITE	g_pD3DXSprite;
extern LPDIRECT3DDEVICE9 g_pD3DDev;

void TEXTURE_BLUR::rotate_texture(void) {
	LPDIRECT3DTEXTURE9 swap;
	if (m_mutex.lock(INFINITE) == false) {
		swap = m_pTexBlur[BLUR_TEXTURE_MAX-1];
		for (int i = BLUR_TEXTURE_MAX-1; i > 0; i--)
			m_pTexBlur[i] = m_pTexBlur[i-1];
		m_pTexBlur[0] = swap;
		m_mutex.unlock();
	}
}

HRESULT TEXTURE_BLUR::CreateTexture( LPDIRECT3DDEVICE9 *pD3DDev ) {
	HRESULT hr;
	if (m_mutex.lock(INFINITE) == false) {
		for (int i = 0; i < BLUR_TEXTURE_MAX; i++){
			hr = (*pD3DDev)->CreateTexture( TEXTURE_WIDTH , TEXTURE_HEIGHT , 1 , D3DUSAGE_DYNAMIC , D3DFMT_A8R8G8B8 , D3DPOOL_DEFAULT , &m_pTexBlur[i] , NULL );
			if( hr ) {
				m_mutex.unlock();
				return hr;
			}
		}
		m_mutex.unlock();
	}
	ClearTexture();
	return 0;
}

void TEXTURE_BLUR::ReleaseTexture(void) {
	if (m_mutex.lock(INFINITE) == false) {
		for (int i = 0; i < BLUR_TEXTURE_MAX; i++)
			SAFE_RELEASE( m_pTexBlur[i] );
		m_mutex.unlock();
	}
}

void TEXTURE_BLUR::RenderBlur(RECT *SrcRc, D3DXVECTOR3 *vecCenter, D3DXVECTOR3 *vecPos, int type) {
	g_pD3DDev->Clear( 0 , NULL , D3DCLEAR_TARGET , D3DCOLOR_XRGB(0,0,0) , 1.0f , 0 );
	if (m_mutex.lock(INFINITE) == false) {
		if ( m_blur_type == BLUR_TYPE_NONE) {
			g_pD3DXSprite->Draw( m_pTexBlur[0] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 255) );
		}
		else {
			// テクスチャのブレンディング方法を定義する
			g_pD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);// 引数の成分を乗算する
			g_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			g_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

			// アルファ合成の設定
			g_pD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			g_pD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		    g_pD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			
			switch (m_blur_type) {
			case BLUR_TYPE_GBCMODE:
				g_pD3DXSprite->Draw( m_pTexBlur[0] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 152) );
				g_pD3DXSprite->Draw( m_pTexBlur[1] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 152) );
				break;
			case BLUR_TYPE_GBMODE:
				g_pD3DDev->Clear( 0 , NULL , D3DCLEAR_TARGET , D3DCOLOR_XRGB(32,76,16) , 1.0f , 0 );
				g_pD3DXSprite->Draw( m_pTexBlur[0] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 56) );
				g_pD3DXSprite->Draw( m_pTexBlur[1] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 56) );
				g_pD3DXSprite->Draw( m_pTexBlur[2] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 22) );
				g_pD3DXSprite->Draw( m_pTexBlur[3] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 22) );
				g_pD3DXSprite->Draw( m_pTexBlur[4] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 12) );
				g_pD3DXSprite->Draw( m_pTexBlur[5] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 12) );
				g_pD3DXSprite->Draw( m_pTexBlur[6] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 8) );
				g_pD3DXSprite->Draw( m_pTexBlur[7] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(200, 200, 200, 8) );
				break;
			case BLUR_TYPE_GBPMODE:
				g_pD3DXSprite->Draw( m_pTexBlur[0] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 100) );
				g_pD3DXSprite->Draw( m_pTexBlur[1] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 100) );
				g_pD3DXSprite->Draw( m_pTexBlur[2] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 20) );
				g_pD3DXSprite->Draw( m_pTexBlur[3] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 20) );
				g_pD3DXSprite->Draw( m_pTexBlur[4] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 8) );
				g_pD3DXSprite->Draw( m_pTexBlur[5] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 8) );
				break;
			case BLUR_TYPE_WSCMODE:
				g_pD3DXSprite->Draw( m_pTexBlur[0] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 51) );
				g_pD3DXSprite->Draw( m_pTexBlur[1] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 51) );
				g_pD3DXSprite->Draw( m_pTexBlur[2] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 51) );
				g_pD3DXSprite->Draw( m_pTexBlur[3] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 51) );
				g_pD3DXSprite->Draw( m_pTexBlur[4] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 51) );
				break;
			default:
				g_pD3DXSprite->Draw( m_pTexBlur[0] , SrcRc , vecCenter , vecPos , D3DCOLOR_RGBA(255, 255, 255, 255) );
				break;
			}
		}
		m_mutex.unlock();
	}
}

HRESULT	TEXTURE_BLUR::SetTexture(const BYTE* p_ScrData) {
	HRESULT hr = 0;

	if (m_pTexBlur[0] == NULL) return -1;
		
	if (m_mutex.lock(INFINITE) == false) {
		rotate_texture();
		hr = BufToTex(p_ScrData, m_pTexBlur[0]);
		m_mutex.unlock();
	}

	return hr;
}

HRESULT TEXTURE_BLUR::ClearTexture(void){
	HRESULT hr;
	if (m_mutex.lock(INFINITE) == false){
		for (int i = 0; i < BLUR_TEXTURE_MAX; i++){
			hr = ClrTex(m_pTexBlur[i]);
			if (hr) {m_mutex.unlock(); return hr;}
		}
		m_mutex.unlock();
	}
	return 0;
}
