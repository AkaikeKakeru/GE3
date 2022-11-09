#pragma once
#include "DirectXBasis.h"

class DrawBasis{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //基本的なメンバ関数
	void Initialize(DirectXBasis* dXBas);

	//描画準備
	void PrepareDraw();
	//描画後処理
	void PostDraw();
private: //固有のメンバ関数

	//頂点バッファビューの作成
	void CreateVertexBufferView(DirectXBasis* dXBas);

	//シェーダファイルを読み込み、コンパイルする
	void CompileShaderFile();

	//頂点レイアウトを組み立てる
	void AssembleVertexLayout();

	//グラフィックスパイプラインを生成
	void CreateGraphicsPipeline(DirectXBasis* dXBas);
	//グラフィックスパイプラインデスクの中身を設定
	void SettingGraphicsPipelineDesc();
	//ルートシグネチャを生成
	void CreateRootSignature(DirectXBasis* dXBas);

public: //ゲッター
	DirectXBasis* GetDXBasis() const { return dXBas_; }
	D3D12_VERTEX_BUFFER_VIEW GetVBView() const { return vbView_; }
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineState_; }
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

private:
	static const int ElementDescNum = 1;//inputLayout_のエレメント数
	static const int VerticesNum = 3;//verticesの頂点数

	DirectXBasis* dXBas_ = nullptr; //DirectX基盤

	ComPtr<ID3D12Resource> vertBuff = nullptr;//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_{};//頂点バッファビュー

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//エラーオブジェクト

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//頂点レイアウト

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//グラフィックスパイプラインデスク

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//ルートシグネチャ

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//パイプラインステート
};