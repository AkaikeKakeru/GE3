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
	void Initialize(const wchar_t* vsFile,const wchar_t* psFile);

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


	///定数バッファ関連
	//定数バッファ生成
	void CreateConstBuffer();

	///定数バッファ生成の中身
	//マテリアル
	void CreateConstBufferMaterial();

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
};