#include "WorldTransform.h"

void WorldTransform::Initialize(){
	//�A�t�B���ϊ����
	scale_ = { 1,1,1 };
	rotation_ = { 0,0,0 };
	position_ = { 0,0,0 };

	matWorld_ = Mat4Identity();
}

void WorldTransform::RecalculationMatWorld(){
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

void WorldTransform::TransferMatWorld(){
	//�萔�o�b�t�@�փf�[�^�]��
	constMapTransform_->mat = matWorld_ *
		viewPro_->GetViewProjection().matView_*
		viewPro_->GetViewProjection().matPro_;
}