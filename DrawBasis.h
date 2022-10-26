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
	//インスタンスのポインタを読み込んで、メンバ変数にセットする
	void LoadInstance(DirectXBasis* dXBas);

	//頂点バッファビューの作成
	void CreateVertexBufferView();
	
	//シェーダファイルを読み込み、コンパイルする
	void CompileShaderFile();

	//頂点レイアウトを組み立てる
	void InputVertexLayout();

	//グラフィックスパイプラインを生成
	void CreateGraphicsPopeline();
	//グラフィックスパイプラインデスクの中身を設定
	void SettingGraphicsPopelineDesc();
	//ルートシグネチャを生成
	void CreateRootSignature();

private:
	static const int ElementDescNum = 1;//inputLayout_のエレメント数

	DirectXBasis* dXBas_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//エラーオブジェクト

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//頂点レイアウト

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//グラフィックスパイプラインデスク
};