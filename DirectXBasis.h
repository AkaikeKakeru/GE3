#pragma once

class DirectXBasis{
public://基本的な関数
public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

	//初期化
	void Initialize();
};