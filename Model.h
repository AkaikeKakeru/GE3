#pragma once
#include "Vector4.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <DirectXTex.h>
#include <wrl.h>

#include "WorldTransform.h"
#include "ViewProjection.h"

using namespace DirectX;

class Model {
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

public://メンバ関数
	void Initialize(Model* model, ID3D12Device* device);
	void InitializeTexture(TextureData* textureData, const wchar_t* szFile);
	void TransferTextureBuffer(TextureData* textureData, ID3D12Device* device);
	void SetInitialize(Model* model, ID3D12Device* device, int objectNum);
	void Update(Model* model);

	void Draw(Model* model, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
		D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

private://メンバ変数

	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffTransform_ = {};

	//定数バッファマップ(行列用)
	ConstBufferDataTransform* constMapTransform_ = {};

	//定数バッファ(ビュープロジェクション)
	ViewProjection* viewProjection_ = nullptr;

	//ワールド変換
	//アフィン変換情報
	WorldTransform worldTransform_{};

	//親オブジェクトへのポインタ
	Model* parent_ = nullptr;
};