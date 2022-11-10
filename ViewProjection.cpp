#include "ViewProjection.h"
#include "WinApp.h"
#include <DirectXMath.h>
using namespace DirectX;

void ViewProjection::Initialize() {
	//今だけは致し方なく宣言(後々なくしたい)
	XMMATRIX xmMatPro; //xmプロジェクション行列
	XMMATRIX xmMatView; //xmビュー行列
	XMFLOAT3 xmEye_;	//xm視点座標
	XMFLOAT3 xmTarget_;	//xm注視点座標
	XMFLOAT3 xmUp_;		//xm上方向ベクトル

	//初期化
	cameraStatus_.eye_ = { 0, 0, -100 };	//視点座標
	cameraStatus_.target_ = { 0, 0, 0 };	//注視点座標
	cameraStatus_.up_ = { 0, 1, 0 };		//上方向ベクトル

	scale_ = { 1,1,1 };
	rotation_ = { 0, 0, 0 };
	position_ = { 0,0,0 };

	axisX_ = { 0,0,0 };
	axisY_ = { 0,0,0 };
	axisZ_ = { 0,0,0 };

	cameraMoveVal_ = { 0,0,0 };

#pragma region 投資投影変換行列の計算
	xmMatPro =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//上下画角45度
			(float)WinApp::WinWidth / WinApp::WinHeight,//アスペクト比(画面横幅/画面縦幅)
			0.1f, 1000.0f
		);//前端、奥端

#pragma region ビュー行列の作成
	xmEye_ = { cameraStatus_.eye_.x,cameraStatus_.eye_.y,cameraStatus_.eye_.z };	//視点座標
	xmTarget_ = { cameraStatus_.target_.x,cameraStatus_.target_.y,cameraStatus_.target_.z };	//注視点座標
	xmUp_ = { cameraStatus_.up_.x,cameraStatus_.up_.y,cameraStatus_.up_.z };		//上方向ベクトル

	xmMatView = XMMatrixLookAtLH(
		XMLoadFloat3(&xmEye_),
		XMLoadFloat3(&xmTarget_),
		XMLoadFloat3(&xmUp_));

	for (int i = 0; i < 4; i++) {
		viewPro_.matPro_.m[i][0] = XMVectorGetX(xmMatPro.r[i]);
		viewPro_.matPro_.m[i][1] = XMVectorGetY(xmMatPro.r[i]);
		viewPro_.matPro_.m[i][2] = XMVectorGetZ(xmMatPro.r[i]);
		viewPro_.matPro_.m[i][3] = XMVectorGetW(xmMatPro.r[i]);

		viewPro_.matView_.m[i][0] = XMVectorGetX(xmMatView.r[i]);
		viewPro_.matView_.m[i][1] = XMVectorGetY(xmMatView.r[i]);
		viewPro_.matView_.m[i][2] = XMVectorGetZ(xmMatView.r[i]);
		viewPro_.matView_.m[i][3] = XMVectorGetW(xmMatView.r[i]);
	}
#pragma endregion
}

void ViewProjection::Update() {
	//今だけは致し方なく宣言(後々なくしたい)
	XMMATRIX xmMatView; //xmビュー行列
	XMFLOAT3 xmEye_;	//xm視点座標
	XMFLOAT3 xmTarget_;	//xm注視点座標
	XMFLOAT3 xmUp_;		//xm上方向ベクトル

	xmEye_ = { cameraStatus_.eye_.x,cameraStatus_.eye_.y,cameraStatus_.eye_.z };	//視点座標
	xmTarget_ = { cameraStatus_.target_.x,cameraStatus_.target_.y,cameraStatus_.target_.z };	//注視点座標
	xmUp_ = { cameraStatus_.up_.x,cameraStatus_.up_.y,cameraStatus_.up_.z };		//上方向ベクトル

	xmMatView = XMMatrixLookAtLH(
		XMLoadFloat3(&xmEye_),
		XMLoadFloat3(&xmTarget_),
		XMLoadFloat3(&xmUp_));

	for (int i = 0; i < 4; i++) {
		viewPro_.matView_.m[i][0] = XMVectorGetX(xmMatView.r[i]);
		viewPro_.matView_.m[i][1] = XMVectorGetY(xmMatView.r[i]);
		viewPro_.matView_.m[i][2] = XMVectorGetZ(xmMatView.r[i]);
		viewPro_.matView_.m[i][3] = XMVectorGetW(xmMatView.r[i]);
	}
}

void ViewProjection::CreateCameraCoordinateAxis(Vector3 eye, Vector3 target, Vector3 up) {
	//視点→注視点へのベクトルをZ軸ベクトルとする。
	axisZ_ = Vec3Normalize(target - eye);
	//上方向ベクトルとZ軸ベクトルの外積をX軸ベクトルとする。
	axisX_ = Vec3Normalize(Vec3Cross(up, axisZ_));
	//Z軸ベクトルとX軸ベクトルの外積をY軸ベクトルとする。
	axisY_ = Vec3Cross(axisZ_, axisX_);
}

void ViewProjection::UpdateMatWorld() {
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

void ViewProjection::CreateMatView() {
	cameraMoveVal_ = {
		Vec3Dot(cameraStatus_.eye_,axisX_),
		Vec3Dot(cameraStatus_.eye_,axisY_),
		Vec3Dot(cameraStatus_.eye_,axisZ_)
	};

	viewPro_.matView_ = {
		axisX_.x,axisX_.y,axisX_.z,0,
		axisY_.x,axisY_.y,axisY_.z,0,
		axisZ_.x,axisZ_.y,axisZ_.z,0,
		cameraMoveVal_.x,cameraMoveVal_.y,cameraMoveVal_.z,1
	};

	viewPro_.matView_ = Mat4Inverse(viewPro_.matView_);
}