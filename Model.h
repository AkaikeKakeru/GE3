#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "Input.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Model {
public:
	//����05-02�Œǉ�
#pragma region 3D�ϊ��s��
	//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
	struct ConstBufferDataTransform_ {
		XMMATRIX mat; //3D�ϊ��s��
	};

public:
	void Initialize(ID3D12Device* device, Input* input, int objectNum);

	void Update(XMMATRIX& matView, XMMATRIX& matProjection);

	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
		D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

private:
	//���W����
	void UpdateObjectPosition();
	//��]����
	void UpdateObjectRotation();
	//�I�u�W�F�N�g����
	void UpdateObjectControll();

public:
	ConstBufferDataTransform_* GetConstMapTransform() {
		return constMapTransform_;
	};

private:
	//Model* object_ = nullptr;

	Input* input_ = nullptr;

	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffTransform = {};

	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform_* constMapTransform_ = {};

	//�A�t�B���ϊ����
	XMFLOAT3 scale_ = { 1,1,1 };
	XMFLOAT3 rotation_ = { 0,0,0 };
	XMFLOAT3 position_ = { 0,0,0 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld_ = {};

	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Model* parent_ = nullptr;
};