#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class ViewProjection {

public:
	void Initialize();
public:
	XMMATRIX matProjection_; //�v���W�F�N�V�����s��

	XMMATRIX matView_; //�r���[�s��

	XMFLOAT3 eye_;	//���_���W
	XMFLOAT3 target_;	//�����_���W
	XMFLOAT3 up_;		//������x�N�g��
};