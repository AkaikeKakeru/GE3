#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class ViewProjection {
public: //構造体
	//定数バッファ用データ構造体(ビュープロジェクション)
	typedef struct ConstBufferDataViewProjection {
		Matrix4 matView_; //ビュー行列
		Matrix4 matPro_; //プロジェクション行列
	}ConstBufferDataViewProjection;

	//カメラの状態
	typedef struct CameraStatus {
		Vector3 eye_;	//視点座標
		Vector3 target_;	//注視点座標
		Vector3 up_;		//上方向ベクトル
	}CameraStatus;

public: // 基本的なメンバ関数
	void Initialize();
	void Update();

private: //固有のメンバ関数
	void CreateCameraCoordinateAxis(Vector3 eye,Vector3 target,Vector3 up);
	void UpdateMatWorld();

public: //ゲッター
	ConstBufferDataViewProjection GetViewProjection() const { return viewPro_; };

public: //セッター
	void SetCameraEye(Vector3 eye) { cameraStatus_.eye_ = eye; };
	void SetCameraTarget(Vector3 target) { cameraStatus_.target_ = target; };
	void SetCameraUp(Vector3 up) { cameraStatus_.up_ = up; };

private: //メンバ変数
	ConstBufferDataViewProjection viewPro_ = {};
	CameraStatus cameraStatus_ = {};

	//アフィン変換情報
	Vector3 scale_ = {};
	Vector3 rotation_ = {};
	Vector3 position_ = {};

	//ワールド変換行列
	Matrix4 matWorld_ = {};
};