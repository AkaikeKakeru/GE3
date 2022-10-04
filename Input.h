#pragma once
#include <windows.h>
#include <wrl.h>
//using namespace Microsoft::WRL;

#define DIRECT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

//入力
class Input
{
	//namespaceの省略
public:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

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
