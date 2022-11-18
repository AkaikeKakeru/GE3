#pragma once
#include <DirectXTex.h>
#include "DirectXBasis.h"
#include "Matrix4.h"
#include "Vector4.h"

using namespace DirectX;

class DrawBasis {
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //構造体
	//定数バッファ用データ構造体(マテリアル)
	struct ConstBufferDataMaterial {
		Vector4 color; //色(RGBA)
	};

	//定数バッファ用データ構造体(3D変換行列)
	struct ConstBufferDataTransform {
		Matrix4 mat; //3D変換行列
	};

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
	//ルートパラメータの設定
	void SettingRootParameter();

	//ルートシグネチャを生成
	void CreateRootSignature(DirectXBasis* dXBas);

	//定数バッファの生成
	void CreateConstBuffer();

	//テクスチャ初期化
	void initializeTexture();

	///テクスチャバッファ
	//テクスチャバッファ生成
	void CreateTextureBuffer();
	//テクスチャバッファ転送
	void TransferTextureBuffer();

	//デスクリプタヒープ生成
	void CreateDescriptorHeap();

	//シェーダリソースビュー生成
	void CreateShagerResourceView();

	//デスクリプタレンジの設定
	void SettingDescriptorRange();

	//テクスチャサンプラーの設定
	void SettingTextureSampler();

	//ワールド変換行列の作成
	void CreateMatWorld();

public: //ゲッター
	DirectXBasis* GetDXBasis() const { return dXBas_; }
	D3D12_VERTEX_BUFFER_VIEW GetVBView() const { return vbView_; }
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineState_; }
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

private:
	static const int ElementDescNum = 2;//inputLayout_のエレメント数
	static const int VerticesNum = 4;//verticesの頂点数
	static const int RootParamsNum = 3;//rootparamsのパラメータ数

	//横方向ピクセル
	const size_t textureWidth = 256;
	//縦方向ピクセル
	const size_t textureHeight = 256;
	//配列の要素数
	const size_t imageDataCount = textureWidth * textureHeight;

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	DirectXBasis* dXBas_ = nullptr; //DirectX基盤

	ComPtr<ID3D12Resource> vertBuff = nullptr;//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW vbView_{};//頂点バッファビュー
	D3D12_RESOURCE_DESC resDesc_{};//リソースデスク

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//エラーオブジェクト

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//頂点レイアウト

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//グラフィックスパイプラインデスク

	D3D12_ROOT_PARAMETER rootParams_[RootParamsNum]{};//ルートパラメータ

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//ルートシグネチャ

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//パイプラインステート

	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;//マテリアル定数バッファ
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;//マテリアル定数マップ
	ComPtr<ID3D12Resource> constBuffTransform_ = nullptr;//3D変換定数バッファ
	ConstBufferDataTransform* constMapTransform_ = nullptr;//3D変換定数マップ

	TexMetadata metadata_;
	ScratchImage scratchImg_;

	ComPtr<ID3D12Resource> texBuff_ = nullptr; //テクスチャバッファ
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_; //SRVハンドル
	ID3D12DescriptorHeap* srvHeap_ = nullptr; //SRVヒープ

	D3D12_DESCRIPTOR_RANGE descriptorRange_{};//デスクリプタレンジ

	D3D12_STATIC_SAMPLER_DESC samplerDesc_{}; //テクスチャサンプラー
};