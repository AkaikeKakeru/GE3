#pragma once
#include <windows.h>
#include <wrl.h>

#include "WinApp.h"

#define DIRECT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>

//入力
class Input {
public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //基本的なメンバ関数
	//初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	//更新
	void Update();

public: //Input固有のメンバ関数
	/// <summary>
	/// キーが押し込まれているかを確認
	/// </summary>
	/// <param name="keyNum">キーの番号</param>
	/// <returns>押し込まれているか</returns>
	bool ifKeyPress(BYTE keyNum);

	/// <summary>
	/// キーが今押されたかを確認
	/// </summary>
	/// <param name="keyNum">キーの番号</param>
	/// <returns>今押されたか</returns>
	bool ifKeyTrigger(BYTE keyNum);

private: //よく使うメンバ変数
	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput = nullptr;
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	//入力されたキー番号
	BYTE key[256] = {};
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp* winApp = nullptr;
};