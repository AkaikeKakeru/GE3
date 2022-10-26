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
	void InputVertexLayout();
	void CreateGraphicsPopeline();

private:
	const int ElementDescNum = 1;//inputLayout_のエレメント数

	DirectXBasis* dXBas_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;//ピクセルシェーダオブジェクト

	D3D12_INPUT_ELEMENT_DESC inputLayout_[];
};