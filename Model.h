#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "Input.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Model {
public:
	//資料05-02で追加
#pragma region 3D変換行列
	//定数バッファ用データ構造体(3D変換行列)
	struct ConstBufferDataTransform_ {
		XMMATRIX mat; //3D変換行列
	};

public:
	void Initialize(ID3D12Device* device, Input* input, int objectNum);

	void Update(XMMATRIX& matView, XMMATRIX& matProjection);

	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
		D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

private:
	//座標操作
	void UpdateObjectPosition();
	//回転操作
	void UpdateObjectRotation();
	//オブジェクト操作
	void UpdateObjectControll();

public:
	ConstBufferDataTransform_* GetConstMapTransform() {
		return constMapTransform_;
	};

private:
	//Model* object_ = nullptr;

	Input* input_ = nullptr;

	//定数バッファ(行列用)
	ComPtr<ID3D12Resource> constBuffTransform = {};

	//定数バッファマップ(行列用)
	ConstBufferDataTransform_* constMapTransform_ = {};

	//アフィン変換情報
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };

	//ワールド変換行列
	XMMATRIX matWorld_ = {};

	//親オブジェクトへのポインタ
	Model* parent_ = nullptr;
};