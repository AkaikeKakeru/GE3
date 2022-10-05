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

//Getter
public:
	HINSTANCE GetHInstance() const { return w.hInstance; }
	HWND GetHwnd() const {return hwnd;}

//外部で使える定数
public:
	//サイズ
	static const int window_width = 1280;
	static const int window_height = 720;

//よく使うメンバ変数
private:
	//クラス設定
	WNDCLASSEX w{};
	//ウィンドウハンドル
	HWND hwnd = nullptr;
};