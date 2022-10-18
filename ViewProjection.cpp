#include "ViewProjection.h"
#include "WinApp.h"

void ViewProjection::Initialize() {

	eye_ = { 0, 0, -100 };	//視点座標
	target_ = { 0, 0, 0 };	//注視点座標
	up_={0, 1, 0};		//上方向ベクトル


#pragma region 投資投影変換行列の計算

	matProjection_ =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//上下画角45度
			(float)WinApp::WinWidth / WinApp::WinHeight,//アスペクト比(画面横幅/画面縦幅)
			0.1f, 1000.0f
		);//前端、奥端

#pragma region ビュー行列の作成

	matView_ = XMMatrixLookAtLH(XMLoadFloat3(&eye_),
		XMLoadFloat3(&target_), XMLoadFloat3(&up_));

#pragma endregion

#pragma endregion
}