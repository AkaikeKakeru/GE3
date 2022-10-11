#pragma once
#include <wrl.h>

class  Drawer
{
public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://基本的なメンバ関数
	void Initialize();

private: //よく使うメンバ変数
	//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob = nullptr;
	//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob = nullptr;

	//グラフィックスパイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
};