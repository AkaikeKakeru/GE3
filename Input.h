#pragma once
#include <windows.h>

//入力
class Input
{
	//基本的なメンバ関数
public:
	//初期化
	void Initialize(HINSTANCE hInstance,
		HWND hwnd);
	//更新
	void Update();

	//よく使うメンバ変数
private:
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

};
