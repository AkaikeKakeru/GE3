#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "ViewProjection.h"

class WorldTransform{
public:
	//定数バッファ用データ構造体(3D変換行列)
	struct ConstBufferDataTransform {
		Matrix4 mat; //3D変換行列
	};

public:
	void Initialize();
	void RecalculationMatWorld();
	void TransferMatWorld();
		
private:
	//アフィン変換情報
	Vector3 scale_{};
	Vector3 rotation_{};
	Vector3 position_{};

	//ワールド変換行列
	Matrix4 matWorld_{};

	//ビュープロジェクション
	ViewProjection* viewPro_{};

	//3D変換定数マップ
	ConstBufferDataTransform* constMapTransform_ = nullptr;
};