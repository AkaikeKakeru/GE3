#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class ViewProjection {
public: //�\����
		//�萔�o�b�t�@�p�f�[�^�\����(�r���[�v���W�F�N�V����)
	typedef struct ConstBufferDataViewProjection {
		Matrix4 matView_; //�r���[�s��
		Matrix4 matPro_; //�v���W�F�N�V�����s��
	}ConstBufferDataViewProjection;

	//�J�����̏��
	typedef struct CameraStatus {
		Vector3 eye_;	//���_���W
		Vector3 target_;	//�����_���W
		Vector3 up_;		//������x�N�g��
	}CameraStatus;

public: // ��{�I�ȃ����o�֐�
		//������
	void Initialize();
	//�X�V
	void Update();

private: //�ŗL�̃����o�֐�
		 //�J�������W���쐬
	void CreateCameraCoordinateAxis(Vector3 eye,Vector3 target,Vector3 up);
	//���[���h���W�s��̍X�V
	void UpdateMatWorld();
	//�r���[�s��쐬
	void CreateMatView();
	//�v���W�F�N�V�����s��쐬
	void CreateMatProjection();

public: //�Z�b�^�[
		//���_�Z�b�^�[
	void SetCameraEye(Vector3 eye) { cameraStatus_.eye_ = eye; };
	//�����_�Z�b�^�[
	void SetCameraTarget(Vector3 target) { cameraStatus_.target_ = target; };
	//������Z�b�^�[
	void SetCameraUp(Vector3 up) { cameraStatus_.up_ = up; };

private: //�����o�ϐ�
		 //�r���[�v���W�F�N�V����
	ConstBufferDataViewProjection viewPro_ = {};
	//�J�������
	CameraStatus cameraStatus_ = {};

	///�A�t�B���ϊ����
	//�X�P�[��
	Vector3 scale_ = {};
	//��]
	Vector3 rotation_ = {};
	//�ʒu
	Vector3 position_ = {};

	//���[���h�ϊ��s��
	Matrix4 matWorld_ = {};

	///�r���[�s��v�f
	//�J����X��
	Vector3 axisX_ = {};
	//�J����Y��
	Vector3 axisY_ = {};
	//�J����Z��
	Vector3 axisZ_ = {};
	////�J�����ړ��l
	//Vector3 cameraMoveVal_ = {};

	///�v���W�F�N�V�����s��v�f
	//����p
	float angle_;
	//�A�X�y�N�g��
	float aspect_;
	//far�N���b�v�l
	float far_;
	//near�N���b�v�l
	float near_;
};