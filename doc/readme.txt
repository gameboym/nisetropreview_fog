初めに
　このプログラムは、ピピン氏及びMM氏が開発された「偽トロキャプチャ」及び「偽トロプレビュー」のプログラムソースを改変し作成されています。
　ソースについては、偽トロプレビューのライセンスであるLGPLに基づいて自由に変更して構いません。
　一応両名に対して報告を行った方がいいかもしれません。
　
　・ピピン氏のブログ
　偽トロキャプチャと車とかとか	http://pipin.blog.eonet.jp/

　・MM氏が配布していた場所（閉鎖中）
　偽トロプレビュー置き場		http://jacobi.sakura.ne.jp/nisetro.html

注意事項
　・DirectX9を使用しているため、DLLファイルが無い場合は以下のURLからランタイムをインストールして下さい。
　　http://www.microsoft.com/downloads/details.aspx?displaylang=ja&FamilyID=2da43d38-db71-4c1b-bc6a-9b6652cd92a3

開発ツール＆ライブラリ
　Microsoft Visual C++ 2010 Express Edition
　Microsoft DirectX SDK
　Microsoft Windows SDK

動作確認ＯＳ
　Windows XP Home

コマンドライン
　--dropframe           エラーフレームの描画方法を表示しない
　--topwindow           常に手前に表示する
　--scale 数字          画面の拡大率を指定
　--rotation-left       左回転させて表示する
　--rotation-right      右回転させて表示する
　--frameskip-60fps     フレームスキップ(60fpsで表示)
　--frameskip-30fps     フレームスキップ(30fpsで表示)
　--frameskip-20fps     フレームスキップ(20fpsで表示)
　--frameskip-15fps     フレームスキップ(15fpsで表示)
　--cusb2id 数字        カメレオンＵＳＢ FX2のIDを選択

説明
　manualフォルダ内のindex.htmlを見てください。

連絡先
　げーむぼーいまにあ(配布場所): http://blogs.dion.ne.jp/sorairo/
　twitter: @gameboym
　mail: gameboymania@gmail.com

変更点
 2012/01/18
 　・「ゲームボーイカラー」「ネオジオポケットカラー（新型）」「スワンクリスタル」に対応。それに伴いDSのプレビュー機能を削除。
 　　（DSで使いたい場合はオリジナルの偽トロプレビューを使用してください）
 　・ffmpegに関するコードを削除したため（後ほど復活予定）録画機能は現時点で使用不可。
 　・残像機能の追加（高速点滅による半透明表現を再現するため実装）
 　・アイコンを変更（仮）
 