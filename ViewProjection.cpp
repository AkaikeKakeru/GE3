#include "ViewProjection.h"
#include "WinApp.h"

void ViewProjection::Initialize() {

	eye_ = { 0, 0, -100 };	//���_���W
	target_ = { 0, 0, 0 };	//�����_���W
	up_={0, 1, 0};		//������x�N�g��


#pragma region �������e�ϊ��s��̌v�Z

	matProjection_ =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//�㉺��p45�x
			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
			0.1f, 1000.0f
		);//�O�[�A���[

#pragma region �r���[�s��̍쐬

	matView_ = XMMatrixLookAtLH(XMLoadFloat3(&eye_),
		XMLoadFloat3(&target_), XMLoadFloat3(&up_));

#pragma endregion

#pragma endregion
}