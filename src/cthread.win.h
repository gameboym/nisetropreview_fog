// 日本語
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <process.h>

#ifndef __CTHREAD_WIN_H__
#define __CTHREAD_WIN_H__

/*!
 * @brief	スレッド用クラス(Windows用)
 * */
class CTHREAD_WIN
{
private:
	//! 待機時間（ms）
	int m_iWait;
	
	//! ハンドル
	HANDLE m_hThread;
	
	//! ループ用フラグ
	bool m_bWork;

	//! スレッドループ用関数
	static unsigned int __stdcall thread_func( void* pData );
protected:

	//! 継承用関数
	virtual unsigned int ThreadFunction( void ) = 0;

public:

	//! コンストラクタ
	CTHREAD_WIN();
	//! デストラクタ
	~CTHREAD_WIN();

	//! スレッド起動
	void startThread( void );

	//! スレッド停止
	void stopThread( void );

	//! 現在の状態を取得
	bool IsWork( void ){return m_bWork;}
	
	//! スレッドループの待機時間を取得(ms)
	int getSleepTime( void ){ return m_iWait; }
	
	//! スレッドループの待機時間を設定(ms)
	int setSleepTime( int iWait ){ if(iWait > 0 )m_iWait = iWait;  return m_iWait; }
};


#endif


