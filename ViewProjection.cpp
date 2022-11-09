#include "ViewProjection.h"

void ViewProjection::Initialize(){
#pragma region 投資投影変換行列の計算

	ConstBufferDataViewProjection viewProjection;
	XMMATRIX matPro;
	matPro =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//上下画角45度
			(float)WinApp::WinWidth / WinApp::WinHeight,//アスペクト比(画面横幅/画面縦幅)
			0.1f, 1000.0f
		);//前端、奥端


#pragma region ビュー行列の作成
	XMMATRIX matView;
	//Vector3 eye(0, 0, -100);	//視点座標
	//Vector3 target(0, 0, 0);	//注視点座標
	//Vector3 up(0, 1, 0);		//上方向ベクトル

	XMFLOAT3 eye(0, 0, -100);	//視点座標
	XMFLOAT3 target(0, 0, 0);	//注視点座標
	XMFLOAT3 up(0, 1, 0);		//上方向ベクトル


	matView = XMMatrixLookAtLH(
		XMLoadFloat3(&eye),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up));

	for (int i = 0; i < 4; i++) {
		viewProjection.projection.m[i][0] = XMVectorGetX(matPro.r[i]);
		viewProjection.projection.m[i][1] = XMVectorGetY(matPro.r[i]);
		viewProjection.projection.m[i][2] = XMVectorGetZ(matPro.r[i]);
		viewProjection.projection.m[i][3] = XMVectorGetW(matPro.r[i]);

		viewProjection.view.m[i][0] = XMVectorGetX(matView.r[i]);
		viewProjection.view.m[i][1] = XMVectorGetY(matView.r[i]);
		viewProjection.view.m[i][2] = XMVectorGetZ(matView.r[i]);
		viewProjection.view.m[i][3] = XMVectorGetW(matView.r[i]);
	}
	//配列内の全オブジェクトに対して
	for (int i = 0; i < _countof(object3ds); i++) {
		object3ds[i].viewProjection_ = viewProjection;
	}
#pragma endregion

}

void ViewProjection::Update(){
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
		XMLoadFloat3(&target), XMLoadFloat3(&up));

	for (int i = 0; i < 4; i++) {
		viewProjection.view.m[i][0] = XMVectorGetX(matView.r[i]);
		viewProjection.view.m[i][1] = XMVectorGetY(matView.r[i]);
		viewProjection.view.m[i][2] = XMVectorGetZ(matView.r[i]);
		viewProjection.view.m[i][3] = XMVectorGetW(matView.r[i]);
	}
}