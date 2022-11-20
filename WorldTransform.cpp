#include "WorldTransform.h"

void WorldTransform::Initialize(){
	//アフィン変換情報
	scale_ = { 1,1,1 };
	rotation_ = { 0,0,0 };
	position_ = { 0,0,0 };

	matWorld_ = Mat4Identity();
}

void WorldTransform::RecalculationMatWorld(){
	Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	//スケール、回転、平行移動行列の計算
	matScale = Mat4Scale(scale_);
	matRota = Mat4Identity();

	matZ = Mat4RotationZ(rotation_.z);
	matX = Mat4RotationX(rotation_.x);
	matY = Mat4RotationY(rotation_.y);

	matRota = matRota * matZ * matX * matY;

	matTrans = Mat4Translation(position_);

	//ワールド行列の合成
	matWorld_ = Mat4Identity();	//変形リセット

	matWorld_ *= matScale;	//ワールド行列のスケーリングを反映
	matWorld_ *= matRota;	//ワールド行列に回転を反映
	matWorld_ *= matTrans;	//ワールド行列に平行移動を反映
}

void WorldTransform::TransferMatWorld(){
	//定数バッファへデータ転送
	constMapTransform_->mat = matWorld_ *
		viewPro_->GetViewProjection().matView_*
		viewPro_->GetViewProjection().matPro_;
}