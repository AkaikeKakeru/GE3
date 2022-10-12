#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "DirectXBasis.h"
#include <DirectXTex.h>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

using namespace DirectX;
class  Drawer
{
public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //�\����
	//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color; //�F(RGBA)
	};

	//�e�N�X�`���f�[�^
	struct TextureData
	{
		TexMetadata metadata{};
		ScratchImage scratchImg{};
		ScratchImage mipChine{};

		D3D12_HEAP_PROPERTIES textureHeapProp{};
		D3D12_RESOURCE_DESC textureResourceDesc{};

		//�e�N�X�`���o�b�t�@�̐���
		ComPtr<ID3D12Resource> texBuff = nullptr;
	};

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT3 normal;	//�@���x�N�g��
		XMFLOAT2 uv;		//uv���W
	};

private: //���_�f�[�^��
	

public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://��{�I�ȃ����o�֐�

	void Initialize(DirectXBasis* dXBas,
		const wchar_t* vsFile,const wchar_t* psFile);

	
	void Update();

private: //�ŗL�̃����o�ϐ�

	void VertexBufferInitialize();
	void TextureInitialize();

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


	//
	void TransferVertices();

	//�e�N�X�`���̏�����
	void InitializeTexture(TextureData* textureData, 
		const wchar_t* szFile);
	//�e�N�X�`���o�b�t�@�̓]��
	void TransferTextureBuffer(TextureData* textureData, 
		ID3D12Device* device);

public: //�Q�b�^
	D3D12_VERTEX_BUFFER_VIEW &GetVBView() { return vbView_; }
	D3D12_INDEX_BUFFER_VIEW &GetIBView() { return ibView_; }

	std::vector<unsigned short> GetIndices() { return indices_; }

private: //�悭�g�������o�ϐ�
	DirectXBasis* dXBas_ = nullptr;

	std::vector<Vertex> vertices_;

	std::vector<unsigned short> indices_;

	//���\�[�X�f�X�N
	D3D12_RESOURCE_DESC resDesc_{};
	//���_�o�b�t�@�̃q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES heapProp_{};
	bool ifOneTextureNum_ = true;

	//���_�}�b�v
	Vertex* vertMap_ = nullptr;

	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_{};


	//�f�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* srvHeap_ = nullptr;
	//�C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	//�C���N�������g�T�C�Y
	UINT incrementSize_;

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