#pragma once

#include "Matrix4.h"
#include "Vector3.h"

class ViewProjection{

public: //�\����
	//�萔�o�b�t�@�p�f�[�^�\����(�r���[�v���W�F�N�V����)
	typedef struct ConstBufferDataViewProjection {
		Matrix4 matView_; //�r���[�s��
		Matrix4 matPro_; //�v���W�F�N�V�����s��
	}ConstBufferDataViewProjection;

	//�J�����̏��
	typedef struct CameraStatus{
		Vector3 eye_;	//���_���W
		Vector3 target_;	//�����_���W
		Vector3 up_;		//������x�N�g��
	}CameraStatus;

public: // ��{�I�ȃ����o�֐�
	void Initialize();
	void Update();

public: //�Q�b�^�[
	ConstBufferDataViewProjection GetViewProjection() const { return viewPro_; };

public: //�Z�b�^�[
	void SetCameraEye(Vector3 eye) { cameraStatus_.eye_ = eye; };
	void SetCameraTarget(Vector3 target) { cameraStatus_.target_ = target; };
	void SetCameraUp(Vector3 up) { cameraStatus_.up_ = up; };

private: //�����o�ϐ�
	XMMATRIX xmMatPro = {}; //xm�v���W�F�N�V�����s��
	XMMATRIX xmMatView = {}; //xm�r���[�s��

	XMFLOAT3 xmEye_;	//xm���_���W
	XMFLOAT3 xmTarget_;	//xm�����_���W
	XMFLOAT3 xmUp_;		//xm������x�N�g��

	ConstBufferDataViewProjection viewPro_ = {};
	CameraStatus cameraStatus_ = {};
};