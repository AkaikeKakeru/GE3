#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "ViewProjection.h"

class WorldTransform{
public:
	//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
	struct ConstBufferDataTransform {
		Matrix4 mat; //3D�ϊ��s��
	};

public:
	//������
	void Initialize();
	//���[���h�s��Čv�Z
	void RecalculationMatWorld();
	//���[���h�s��]��
	void TransferMatWorld();

public:
	///�A�t�B���ϊ����
	Vector3 scale_;//�X�P�[��
	Vector3 rotation_;//��]
	Vector3 position_;//�ʒu

	//���[���h�ϊ��s��
	Matrix4 matWorld_{};

private:
	//�r���[�v���W�F�N�V����
	ViewProjection* viewPro_{};

	//3D�ϊ��萔�}�b�v
	ConstBufferDataTransform* constMapTransform_ = nullptr;
};