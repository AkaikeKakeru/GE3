#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class  Drawer
{
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

private: //よく使うメンバ変数

	//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> vsBlob_ = nullptr;
	//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;
	//エラーオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;

	//グラフィックスパイプライン
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};
};