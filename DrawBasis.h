#pragma once
#include "DirectXBasis.h"
#include "Vector4.h"

class DrawBasis{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //�\����
	//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
	struct ConstBufferDataMaterial {
		Vector4 color; //�F(RGBA)
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

public: //�Q�b�^�[
	DirectXBasis* GetDXBasis() const { return dXBas_; }
	D3D12_VERTEX_BUFFER_VIEW GetVBView() const { return vbView_; }
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineState_; }
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

private:
	static const int ElementDescNum = 1;//inputLayout_�̃G�������g��
	static const int VerticesNum = 3;//vertices�̒��_��

	DirectXBasis* dXBas_ = nullptr; //DirectX���

	ComPtr<ID3D12Resource> vertBuff = nullptr;//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW vbView_{};//���_�o�b�t�@�r���[

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//���_���C�A�E�g

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//�O���t�B�b�N�X�p�C�v���C���f�X�N

	D3D12_ROOT_PARAMETER rootParam_ = {};//���[�g�p�����[�^

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//���[�g�V�O�l�`��

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//�p�C�v���C���X�e�[�g

	ConstBufferDataMaterial* constMapMaterial_ = nullptr;//�}�e���A���萔�}�b�v
};