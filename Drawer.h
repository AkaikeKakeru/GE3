#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "DirectXBasis.h"
#include <DirectXTex.h>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

using namespace DirectX;
class  Drawer
{
public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //構造体
	//定数バッファ用データ構造体(マテリアル)
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color; //色(RGBA)
	};

	//テクスチャデータ
	struct TextureData
	{
		TexMetadata metadata{};
		ScratchImage scratchImg{};
		ScratchImage mipChine{};

		D3D12_HEAP_PROPERTIES textureHeapProp{};
		D3D12_RESOURCE_DESC textureResourceDesc{};

		//テクスチャバッファの生成
		ComPtr<ID3D12Resource> texBuff = nullptr;
	};

	//頂点データ構造体
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz座標
		XMFLOAT3 normal;	//法線ベクトル
		XMFLOAT2 uv;		//uv座標
	};

private: //頂点データ等
	

public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://基本的なメンバ関数

	void Initialize(DirectXBasis* dXBas,
		const wchar_t* vsFile,const wchar_t* psFile);

	
	void Update();

private: //固有のメンバ変数

	void VertexBufferInitialize();
	void TextureInitialize();

	///グラフィックスパイプライン関連
	//グラフィックスパイプライン設定
	void SetingGraphicsPipeline();

	///グラフィックスパイプライン設定の中身
	//シェーダーファイル読み込み
	void LoadShaderFile(
		const wchar_t* vsFile,
		const wchar_t* psFile);
	//シェーダー設定
	void SetingShader();
	//ラスタライザ設定
	void SetingRasterizer();
	//アルファブレンディング設定
	void SetingAlphaBlend();
	//デプスステンシルステートの設定
	void SetingDepthStencilState();
	//その他の設定
	void SetingOther();

	//デスクリプタレンジの設定
	void SetingDescriptorRange();
	//デスクリプタテーブルの設定
	void SetingDescriptorTable();

	//テクスチャサンプラーの設定
	void SetingTextureSampler();

	//ルートパラメータ設定
	void SetingRootParameter();
	//ルートシグネチャ設定
	void SetingRootSignature();
	//パイプラインステート生成
	void CreatePipelineState();

	///定数バッファ関連
	//定数バッファ生成
	void CreateConstBuffer();

	///定数バッファ生成の中身
	//マテリアル
	void CreateConstBufferMaterial();


	//
	void TransferVertices();

	//テクスチャの初期化
	void InitializeTexture(TextureData* textureData, 
		const wchar_t* szFile);
	//テクスチャバッファの転送
	void TransferTextureBuffer(TextureData* textureData, 
		ID3D12Device* device);

public: //ゲッタ
	D3D12_VERTEX_BUFFER_VIEW &GetVBView() { return vbView_; }
	D3D12_INDEX_BUFFER_VIEW &GetIBView() { return ibView_; }

	std::vector<unsigned short> GetIndices() { return indices_; }

private: //よく使うメンバ変数
	DirectXBasis* dXBas_ = nullptr;

	std::vector<Vertex> vertices_;

	std::vector<unsigned short> indices_;

	//リソースデスク
	D3D12_RESOURCE_DESC resDesc_{};
	//頂点バッファのヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProp_{};
	bool ifOneTextureNum_ = true;

	//頂点マップ
	Vertex* vertMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};


	//デスクリプタヒープ
	ID3D12DescriptorHeap* srvHeap_ = nullptr;
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	//インクリメントサイズ
	UINT incrementSize_;

	//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob_ = nullptr;
	//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;
	//エラーオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;

	//グラフィックスパイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};

	//サンプラーデスク
	D3D12_STATIC_SAMPLER_DESC samplerDesc_{};

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> rootSignature_;

	//パイプラインステート
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	//デスクリプタレンジ(タイプSRV)
	D3D12_DESCRIPTOR_RANGE descriptorRange_{};
	//デスクテーブル(タイプCBV)
	D3D12_DESCRIPTOR_RANGE descRange_{};

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rootParams_[3] = {};

	//マテリアル定数バッファ
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
};