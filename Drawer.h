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
	void Initialize(const wchar_t* vsFile,const wchar_t* psFile);

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


	///�萔�o�b�t�@�֘A
	//�萔�o�b�t�@����
	void CreateConstBuffer();

	///�萔�o�b�t�@�����̒��g
	//�}�e���A��
	void CreateConstBufferMaterial();

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
};