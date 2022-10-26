#pragma once
#include "DirectXBasis.h"

class DrawBasis
{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //��{�I�ȃ����o�֐�
	void Initialize(DirectXBasis* dXBas);

private: //�ŗL�̃����o�֐�
	//�C���X�^���X�̃|�C���^��ǂݍ���ŁA�����o�ϐ��ɃZ�b�g����
	void LoadInstance(DirectXBasis* dXBas);

	//���_�o�b�t�@�r���[�̍쐬
	void CreateVertexBufferView();
	
	//�V�F�[�_�t�@�C����ǂݍ��݁A�R���p�C������
	void CompileShaderFile();

	//���_���C�A�E�g��g�ݗ��Ă�
	void InputVertexLayout();

	//�O���t�B�b�N�X�p�C�v���C���𐶐�
	void CreateGraphicsPopeline();
	//�O���t�B�b�N�X�p�C�v���C���f�X�N�̒��g��ݒ�
	void SettingGraphicsPopelineDesc();
	//���[�g�V�O�l�`���𐶐�
	void CreateRootSignature();

private:
	static const int ElementDescNum = 1;//inputLayout_�̃G�������g��

	DirectXBasis* dXBas_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//���_���C�A�E�g

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//�O���t�B�b�N�X�p�C�v���C���f�X�N
};