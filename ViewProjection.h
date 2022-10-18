#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class ViewProjection {

public:
	void Initialize();
public:
	XMMATRIX matProjection_; //プロジェクション行列

	XMMATRIX matView_; //ビュー行列

	XMFLOAT3 eye_;	//視点座標
	XMFLOAT3 target_;	//注視点座標
	XMFLOAT3 up_;		//上方向ベクトル
};