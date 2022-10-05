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

//よく使うメンバ変数
private:
	//サイズ
	const int window_width = 1280;
	const int window_height = 720;

	//ウィンドウハンドル
	HWND hwnd = nullptr;
};