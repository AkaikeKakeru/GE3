#include "ViewProjection.h"

void ViewProjection::Initialize(){
#pragma region �������e�ϊ��s��̌v�Z

	ConstBufferDataViewProjection viewProjection;
	XMMATRIX matPro;
	matPro =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//�㉺��p45�x
			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
			0.1f, 1000.0f
		);//�O�[�A���[


#pragma region �r���[�s��̍쐬
	XMMATRIX matView;
	//Vector3 eye(0, 0, -100);	//���_���W
	//Vector3 target(0, 0, 0);	//�����_���W
	//Vector3 up(0, 1, 0);		//������x�N�g��

	XMFLOAT3 eye(0, 0, -100);	//���_���W
	XMFLOAT3 target(0, 0, 0);	//�����_���W
	XMFLOAT3 up(0, 1, 0);		//������x�N�g��


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
	//�z����̑S�I�u�W�F�N�g�ɑ΂���
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