#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class ViewProjection{

public: //構造体
	//定数バッファ用データ構造体(ビュープロジェクション)
	typedef struct ConstBufferDataViewProjection {
		Matrix4 matView_; //ビュー行列
		Matrix4 matPro_; //プロジェクション行列
	}ConstBufferDataViewProjection;

	//カメラの状態
	typedef struct CameraStatus{
		Vector3 eye_;	//視点座標
		Vector3 target_;	//注視点座標
		Vector3 up_;		//上方向ベクトル
	}CameraStatus;

public: // 基本的なメンバ関数
	void Initialize();
	void Update();

public: //ゲッター
	ConstBufferDataViewProjection GetViewProjection() const { return viewPro_; };

public: //セッター
	void SetCameraEye(Vector3 eye) { cameraStatus_.eye_ = eye; };
	void SetCameraTarget(Vector3 target) { cameraStatus_.target_ = target; };
	void SetCameraUp(Vector3 up) { cameraStatus_.up_ = up; };

private: //メンバ変数
	XMMATRIX xmMatPro = {}; //xmプロジェクション行列
	XMMATRIX xmMatView = {}; //xmビュー行列

	XMFLOAT3 xmEye_;	//xm視点座標
	XMFLOAT3 xmTarget_;	//xm注視点座標
	XMFLOAT3 xmUp_;		//xm上方向ベクトル

	ConstBufferDataViewProjection viewPro_ = {};
	CameraStatus cameraStatus_ = {};
};