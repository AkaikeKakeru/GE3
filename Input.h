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

	//Input固有のメンバ関数
public:
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
	/// <param name="keyPreNum">1F前のキーの番号</param>
	/// <returns>今押されたか</returns>
	bool ifKeyTrigger(BYTE keyNum, BYTE keyPreNum);

	//よく使うメンバ変数
private:
	//キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	BYTE key[256] = {};
	BYTE keyPre[256] = {};
};
