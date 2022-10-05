#pragma once
#include <Windows.h>

class WinApp {
//静的メンバ関数(static)
public:
	static LRESULT WindowProc(
		HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam);

//基本的なメンバ関数
public:
	//初期化
	void Initialize();
	//更新
	void Update();
	//終了
	void Finalize();

//Getter
public:
	HINSTANCE GetHInstance() const { return w_.hInstance; }
	HWND GetHwnd() const {return hwnd_;}
	MSG GetMsg() const { return msg_; }

//外部で使える定数
public:
	//サイズ
	static const int WinWidth = 1280;
	static const int WinHeight = 720;

//よく使うメンバ変数
private:
	//クラス設定
	WNDCLASSEX w_{};
	//ウィンドウハンドル
	HWND hwnd_ = nullptr;
	//メッセージ
	MSG msg_{};
};