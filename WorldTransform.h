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
	//初期化
	void Initialize();
	//ワールド行列再計算
	void RecalculationMatWorld();
	//ワールド行列転送
	void TransferMatWorld();

public:
	///アフィン変換情報
	Vector3 scale_;//スケール
	Vector3 rotation_;//回転
	Vector3 position_;//位置

	//ワールド変換行列
	Matrix4 matWorld_{};

private:
	//ビュープロジェクション
	ViewProjection* viewPro_{};

	//3D変換定数マップ
	ConstBufferDataTransform* constMapTransform_ = nullptr;
};