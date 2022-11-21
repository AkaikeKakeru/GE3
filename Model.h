#pragma once
#include "Vector4.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <DirectXTex.h>
#include <wrl.h>

#include "WorldTransform.h"
#include "ViewProjection.h"

using namespace DirectX;

class Model {
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: // �\����
		//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
	struct ConstBufferDataMaterial {
		Vector4 color; //�F(RGBA)
	};

	//����05-02�Œǉ�
#pragma region 3D�ϊ��s��
	//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
	struct ConstBufferDataTransform {
		Matrix4 mat; //3D�ϊ��s��
	};

	struct TextureData {
		TexMetadata metadata{};
		ScratchImage scratchImg{};
		ScratchImage mipChine{};

		D3D12_HEAP_PROPERTIES textureHeapProp{};
		D3D12_RESOURCE_DESC textureResourceDesc{};

		//�e�N�X�`���o�b�t�@�̐���
		ComPtr<ID3D12Resource> texBuff = nullptr;
	};

public://�����o�֐�
	void Initialize(Model* model, ID3D12Device* device);
	void InitializeTexture(TextureData* textureData, const wchar_t* szFile);
	void TransferTextureBuffer(TextureData* textureData, ID3D12Device* device);
	void SetInitialize(Model* model, ID3D12Device* device, int objectNum);
	void Update(Model* model);

	void Draw(Model* model, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
		D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

private://�����o�ϐ�

	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffTransform_ = {};

	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform* constMapTransform_ = {};

	//�萔�o�b�t�@(�r���[�v���W�F�N�V����)
	ViewProjection* viewProjection_ = nullptr;

	//���[���h�ϊ�
	//�A�t�B���ϊ����
	WorldTransform worldTransform_{};

	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Model* parent_ = nullptr;
};