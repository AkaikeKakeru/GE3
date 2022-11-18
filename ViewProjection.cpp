#include "ViewProjection.h"
#include "WinApp.h"
#include <DirectXMath.h>
using namespace DirectX;

void ViewProjection::Initialize() {
	//�������͒v�����Ȃ��錾(��X�Ȃ�������)
	XMMATRIX xmMatPro; //xm�v���W�F�N�V�����s��

					   //������
	cameraStatus_.eye_ = { 0, 0, -100 };	//���_���W
	cameraStatus_.target_ = { 0, 0, 0 };	//�����_���W
	cameraStatus_.up_ = { 0, 1, 0 };		//������x�N�g��

	scale_ = { 1,1,1 };
	rotation_ = { 0, 0, 0 };
	position_ = { 0,0,0 };

	axisX_ = { 0,0,0 };
	axisY_ = { 0,0,0 };
	axisZ_ = { 0,0,0 };

	cameraMoveVal_ = { 0,0,0 };

#pragma region �������e�ϊ��s��̌v�Z
	xmMatPro =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//�㉺��p45�x
			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
			0.1f, 1000.0f
		);//�O�[�A���[

#pragma region �r���[�s��̍쐬
	for (int i = 0; i < 4; i++) {
		viewPro_.matPro_.m[i][0] = XMVectorGetX(xmMatPro.r[i]);
		viewPro_.matPro_.m[i][1] = XMVectorGetY(xmMatPro.r[i]);
		viewPro_.matPro_.m[i][2] = XMVectorGetZ(xmMatPro.r[i]);
		viewPro_.matPro_.m[i][3] = XMVectorGetW(xmMatPro.r[i]);
	}

	CreateCameraCoordinateAxis(
		cameraStatus_.eye_,
		cameraStatus_.target_,
		cameraStatus_.up_
	);

	CreateMatView();
#pragma endregion
}

void ViewProjection::Update() {
	CreateCameraCoordinateAxis(
		cameraStatus_.eye_,
		cameraStatus_.target_,
		cameraStatus_.up_
	);

	CreateMatView();
}

void ViewProjection::CreateCameraCoordinateAxis(Vector3 eye, Vector3 target, Vector3 up) {
	//���_�������_�ւ̃x�N�g����Z���x�N�g���Ƃ���B
	axisZ_ = Vec3Normalize(target - eye);
	//������x�N�g����Z���x�N�g���̊O�ς�X���x�N�g���Ƃ���B
	axisX_ = Vec3Normalize(Vec3Cross(up, axisZ_));
	//Z���x�N�g����X���x�N�g���̊O�ς�Y���x�N�g���Ƃ���B
	axisY_ = Vec3Cross(axisZ_, axisX_);
}

void ViewProjection::UpdateMatWorld() {
	Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = Mat4Scale(scale_);
	matRota = Mat4Identity();

	matZ = Mat4RotationZ(rotation_.z);
	matX = Mat4RotationX(rotation_.x);
	matY = Mat4RotationY(rotation_.y);

	matRota = matRota * matZ * matX * matY;

	matTrans = Mat4Translation(position_);

	//���[���h�s��̍���
	matWorld_ = Mat4Identity();	//�ό`���Z�b�g

	matWorld_ *= matScale;	//���[���h�s��̃X�P�[�����O�𔽉f
	matWorld_ *= matRota;	//���[���h�s��ɉ�]�𔽉f
	matWorld_ *= matTrans;	//���[���h�s��ɕ��s�ړ��𔽉f
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
		-cameraMoveVal_.x,-cameraMoveVal_.y,-cameraMoveVal_.z,1
	};
}