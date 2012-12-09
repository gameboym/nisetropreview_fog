
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <windows.h>

#ifndef CMUTEX_WIN_H_
#define CMUTEX_WIN_H_

/*!
 * @brief	ミューテクス用クラス(Windows用)
 * */
class CMUTEX_WIN
{
protected:
	//! ハンドル
	HANDLE m_hMutex;
public:
	//! コンストラクタ
	CMUTEX_WIN();
	//! デストラクタ
	~CMUTEX_WIN();

	//! ミューテクスの作成
	void create( void );

	//! ミューテクスの解放
	void destroy( void );

	//! ロックの取得
	int lock( DWORD dwMSec );

	//! ロックの解放
	int unlock( void );
};



#endif /* CMUTEX_WIN_H_ */
