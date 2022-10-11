#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "DirectXBasis.h"

#pragma comment(lib,"d3d12.lib")

using namespace DirectX;
class  Drawer
{
public: //�\����
	//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color; //�F(RGBA)
	};

public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://��{�I�ȃ����o�֐�
	void Initialize(DirectXBasis* dXBas,const wchar_t* vsFile,const wchar_t* psFile);

private: //�ŗL�̃����o�ϐ�

	///�O���t�B�b�N�X�p�C�v���C���֘A
	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	void SetingGraphicsPipeline();

	///�O���t�B�b�N�X�p�C�v���C���ݒ�̒��g
	//�V�F�[�_�[�t�@�C���ǂݍ���
	void LoadShaderFile(
		const wchar_t* vsFile,
		const wchar_t* psFile);
	//�V�F�[�_�[�ݒ�
	void SetingShader();
	//���X�^���C�U�ݒ�
	void SetingRasterizer();
	//�A���t�@�u�����f�B���O�ݒ�
	void SetingAlphaBlend();
	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	void SetingDepthStencilState();
	//���̑��̐ݒ�
	void SetingOther();

	//�f�X�N���v�^�����W�̐ݒ�
	void SetingDescriptorRange();
	//�f�X�N���v�^�e�[�u���̐ݒ�
	void SetingDescriptorTable();

	//�e�N�X�`���T���v���[�̐ݒ�
	void SetingTextureSampler();

	//���[�g�p�����[�^�ݒ�
	void SetingRootParameter();
	//���[�g�V�O�l�`���ݒ�
	void SetingRootSignature();
	//�p�C�v���C���X�e�[�g����
	void CreatePipelineState();

	///�萔�o�b�t�@�֘A
	//�萔�o�b�t�@����
	void CreateConstBuffer();

	///�萔�o�b�t�@�����̒��g
	//�}�e���A��
	void CreateConstBufferMaterial();

public: //�Q�b�^
	ComPtr<ID3D12RootSignature> GetRootSignature() { return  rootSignature_; }
	ComPtr<ID3D12PipelineState> GetPipelineState() { return  pipelineState_; }
	ComPtr<ID3D12Resource> GetConstBuffMaterial() { return constBuffMaterial_; }

private: //�悭�g�������o�ϐ�
	DirectXBasis* dXBas_ = nullptr;

	//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> vsBlob_ = nullptr;
	//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;
	//�G���[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;

	//�O���t�B�b�N�X�p�C�v���C��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};

	//�T���v���[�f�X�N
	D3D12_STATIC_SAMPLER_DESC samplerDesc_{};

	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature_;

	//�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	//�f�X�N���v�^�����W(�^�C�vSRV)
	D3D12_DESCRIPTOR_RANGE descriptorRange_{};
	//�f�X�N�e�[�u��(�^�C�vCBV)
	D3D12_DESCRIPTOR_RANGE descRange_{};

	//���[�g�p�����[�^
	D3D12_ROOT_PARAMETER rootParams_[3] = {};

	//�}�e���A���萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
};