#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "DirectXBasis.h"

#pragma comment(lib,"d3d12.lib")

using namespace DirectX;
class  Drawer
{
public: //構造体
	//定数バッファ用データ構造体(マテリアル)
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color; //色(RGBA)
	};

public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://基本的なメンバ関数
	void Initialize(DirectXBasis* dXBas,const wchar_t* vsFile,const wchar_t* psFile);

private: //固有のメンバ変数

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

public: //ゲッタ
	ComPtr<ID3D12RootSignature> GetRootSignature() { return  rootSignature_; }
	ComPtr<ID3D12PipelineState> GetPipelineState() { return  pipelineState_; }
	ComPtr<ID3D12Resource> GetConstBuffMaterial() { return constBuffMaterial_; }

private: //よく使うメンバ変数
	DirectXBasis* dXBas_ = nullptr;

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