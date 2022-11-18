#pragma once
#include <DirectXTex.h>
#include "DirectXBasis.h"
#include "Matrix4.h"
#include "Vector4.h"

using namespace DirectX;

class DrawBasis {
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //�\����
	//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
	struct ConstBufferDataMaterial {
		Vector4 color; //�F(RGBA)
	};

	//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
	struct ConstBufferDataTransform {
		Matrix4 mat; //3D�ϊ��s��
	};

public: //��{�I�ȃ����o�֐�
	void Initialize(DirectXBasis* dXBas);

	//�`�揀��
	void PrepareDraw();
	//�`��㏈��
	void PostDraw();
private: //�ŗL�̃����o�֐�

	//���_�o�b�t�@�r���[�̍쐬
	void CreateVertexBufferView(DirectXBasis* dXBas);

	//�V�F�[�_�t�@�C����ǂݍ��݁A�R���p�C������
	void CompileShaderFile();

	//���_���C�A�E�g��g�ݗ��Ă�
	void AssembleVertexLayout();

	//�O���t�B�b�N�X�p�C�v���C���𐶐�
	void CreateGraphicsPipeline(DirectXBasis* dXBas);
	//�O���t�B�b�N�X�p�C�v���C���f�X�N�̒��g��ݒ�
	void SettingGraphicsPipelineDesc();
	//���[�g�p�����[�^�̐ݒ�
	void SettingRootParameter();

	//���[�g�V�O�l�`���𐶐�
	void CreateRootSignature(DirectXBasis* dXBas);

	//�萔�o�b�t�@�̐���
	void CreateConstBuffer();

	//�e�N�X�`��������
	void initializeTexture();

	///�e�N�X�`���o�b�t�@
	//�e�N�X�`���o�b�t�@����
	void CreateTextureBuffer();
	//�e�N�X�`���o�b�t�@�]��
	void TransferTextureBuffer();

	//�f�X�N���v�^�q�[�v����
	void CreateDescriptorHeap();

	//�V�F�[�_���\�[�X�r���[����
	void CreateShagerResourceView();

	//�f�X�N���v�^�����W�̐ݒ�
	void SettingDescriptorRange();

	//�e�N�X�`���T���v���[�̐ݒ�
	void SettingTextureSampler();

	//���[���h�ϊ��s��̍쐬
	void CreateMatWorld();

public: //�Q�b�^�[
	DirectXBasis* GetDXBasis() const { return dXBas_; }
	D3D12_VERTEX_BUFFER_VIEW GetVBView() const { return vbView_; }
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineState_; }
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

private:
	static const int ElementDescNum = 2;//inputLayout_�̃G�������g��
	static const int VerticesNum = 4;//vertices�̒��_��
	static const int RootParamsNum = 3;//rootparams�̃p�����[�^��

	//�������s�N�Z��
	const size_t textureWidth = 256;
	//�c�����s�N�Z��
	const size_t textureHeight = 256;
	//�z��̗v�f��
	const size_t imageDataCount = textureWidth * textureHeight;

	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	DirectXBasis* dXBas_ = nullptr; //DirectX���

	ComPtr<ID3D12Resource> vertBuff = nullptr;//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_{};//���_�o�b�t�@�r���[
	D3D12_RESOURCE_DESC resDesc_{};//���\�[�X�f�X�N

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//���_���C�A�E�g

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//�O���t�B�b�N�X�p�C�v���C���f�X�N

	D3D12_ROOT_PARAMETER rootParams_[RootParamsNum]{};//���[�g�p�����[�^

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//���[�g�V�O�l�`��

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//�p�C�v���C���X�e�[�g

	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;//�}�e���A���萔�o�b�t�@
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;//�}�e���A���萔�}�b�v
	ComPtr<ID3D12Resource> constBuffTransform_ = nullptr;//3D�ϊ��萔�o�b�t�@
	ConstBufferDataTransform* constMapTransform_ = nullptr;//3D�ϊ��萔�}�b�v

	TexMetadata metadata_;
	ScratchImage scratchImg_;

	ComPtr<ID3D12Resource> texBuff_ = nullptr; //�e�N�X�`���o�b�t�@
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_; //SRV�n���h��
	ID3D12DescriptorHeap* srvHeap_ = nullptr; //SRV�q�[�v

	D3D12_DESCRIPTOR_RANGE descriptorRange_{};//�f�X�N���v�^�����W

	D3D12_STATIC_SAMPLER_DESC samplerDesc_{}; //�e�N�X�`���T���v���[
};