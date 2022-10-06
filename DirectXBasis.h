#pragma once
#include "WinApp.h"

class DirectXBasis{
public://基本的なメンバ関数
public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

	//初期化
	void Initialize(WinApp* winApp);

private://よく使うメンバ変数
	WinApp* winApp_ = nullptr;
};