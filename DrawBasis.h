#pragma once
#include "DirectXBasis.h"

class DrawBasis
{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //基本的なメンバ関数
	void Initialize(DirectXBasis* dXBas);

private: //固有のメンバ関数
	void LoadInstance(DirectXBasis* dXBas);
	void CreateVertexBufferView();
	void CompileShaderFile();

private:
	DirectXBasis* dXBas_ = nullptr;
};