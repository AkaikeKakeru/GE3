#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <DirectXTex.h>
#include <wrl.h>

#include "DirectXBasis.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

using namespace DirectX;

class ObjectBasis {
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: // 構造体
	//定数バッファ用データ構造体(マテリアル)
	struct ConstBufferDataMaterial {
		Vector4 color; //色(RGBA)
	};

	//資料05-02で追加
#pragma region 3D変換行列
	//定数バッファ用データ構造体(3D変換行列)
	struct ConstBufferDataTransform {
		Matrix4 mat; //3D変換行列
	};

	struct TextureData {
		TexMetadata metadata{};
		ScratchImage scratchImg{};
		ScratchImage mipChine{};

		D3D12_HEAP_PROPERTIES textureHeapProp{};
		D3D12_RESOURCE_DESC textureResourceDesc{};

		//テクスチャバッファの生成
		ComPtr<ID3D12Resource> texBuff = nullptr;
	};

	//頂点データ構造体
	struct Vertex {
		Vector3 pos;		//xyz座標
		Vector3 normal;	//法線ベクトル
		Vector2 uv;		//uv座標
	};

public://メンバ関数
	void Initialize(ComPtr<ID3D12Device> device);
	void InitializeTexture(const wchar_t* szFile);
	void TransferTextureBuffer();
	void Update();

	void Draw( /*UINT numIndices*/);

	void copyInit(DirectXBasis*dXBas);
	void copyUp();
	void copyDraw();

	void PrepareDraw(ComPtr<ID3D12GraphicsCommandList> cmdList);
	void PostDraw();

public:// 定数
	static const int SurfaceNum = 6;
	static const int VertexNum = 4;
	static const int VeticesNum = SurfaceNum * VertexNum;

	static const int IndexNum = SurfaceNum * 2 * 3;
private://static
	//頂点データ
	static std::vector<Vertex> vertices_;
	//インデックス
	static std::vector<unsigned short> indices_;

	//デバイス
	static ComPtr<ID3D12Device> device_;

	//コマンドリスト
	static ComPtr<ID3D12GraphicsCommandList> cmdList_;

private://メンバ変数
	DirectXBasis* dXBas_ = nullptr;

	D3D12_RESOURCE_DESC resDesc_{};

	Vertex* vertMap_ = nullptr;
	unsigned short* indexMap_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob_ = nullptr;//ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//エラーオブジェクト

	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffMaterial_ = {};
	//定数バッファマップ(行列用)
	ConstBufferDataMaterial* constMapMaterial_ = {};

	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffTransform_ = {};
	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform_ = {};

	ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;

	//定数バッファ(ビュープロジェクション)
	ViewProjection* viewPro_ = nullptr;
	//ワールド変換
	//アフィン変換情報
	WorldTransform worldTransform_{};

	//親オブジェクトへのポインタ
	ObjectBasis* parent_ = nullptr;

	TextureData textureData_;
};