#include "ViewProjection.h"
#include "WinApp.h"
#include <DirectXMath.h>
using namespace DirectX;

void ViewProjection::Initialize() {
	//�������͒v�����Ȃ��錾(��X�Ȃ�������)
	XMMATRIX xmMatPro; //xm�v���W�F�N�V�����s��
	XMMATRIX xmMatView; //xm�r���[�s��
	XMFLOAT3 xmEye_;	//xm���_���W
	XMFLOAT3 xmTarget_;	//xm�����_���W
	XMFLOAT3 xmUp_;		//xm������x�N�g��

	//������
	cameraStatus_.eye_ = { 0, 0, -100 };	//���_���W
	cameraStatus_.target_ = { 0, 0, 0 };	//�����_���W
	cameraStatus_.up_ = { 0, 1, 0 };		//������x�N�g��

#pragma region �������e�ϊ��s��̌v�Z
	xmMatPro =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//�㉺��p45�x
			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
			0.1f, 1000.0f
		);//�O�[�A���[

#pragma region �r���[�s��̍쐬
	xmEye_ = { cameraStatus_.eye_.x,cameraStatus_.eye_.y,cameraStatus_.eye_.z };	//���_���W
	xmTarget_ = { cameraStatus_.target_.x,cameraStatus_.target_.y,cameraStatus_.target_.z };	//�����_���W
	xmUp_ = { cameraStatus_.up_.x,cameraStatus_.up_.y,cameraStatus_.up_.z };		//������x�N�g��

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
	//�������͒v�����Ȃ��錾(��X�Ȃ�������)
	XMMATRIX xmMatView; //xm�r���[�s��
	XMFLOAT3 xmEye_;	//xm���_���W
	XMFLOAT3 xmTarget_;	//xm�����_���W
	XMFLOAT3 xmUp_;		//xm������x�N�g��

	xmEye_ = { cameraStatus_.eye_.x,cameraStatus_.eye_.y,cameraStatus_.eye_.z };	//���_���W
	xmTarget_ = { cameraStatus_.target_.x,cameraStatus_.target_.y,cameraStatus_.target_.z };	//�����_���W
	xmUp_ = { cameraStatus_.up_.x,cameraStatus_.up_.y,cameraStatus_.up_.z };		//������x�N�g��

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

void ViewProjection::CreateCameraCoordinateAxis(Vector3 eye, Vector3 target, Vector3 up){
	Vector3 VecAxisX, VecAxisY, VecAxisZ;

	//���_�������_�ւ̃x�N�g����Z���x�N�g���Ƃ���B
	VecAxisZ = Vec3Normalize(target - eye);
	//������x�N�g����Z���x�N�g���̊O�ς�X���x�N�g���Ƃ���B
	VecAxisX = Vec3Normalize(Vec3Cross(up, VecAxisZ));
	//Z���x�N�g����X���x�N�g���̊O�ς�Y���x�N�g���Ƃ���B
	VecAxisY = Vec3Cross(VecAxisZ, VecAxisX);
}