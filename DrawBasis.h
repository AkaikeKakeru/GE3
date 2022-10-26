#pragma once
#include "DirectXBasis.h"

class DrawBasis
{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //��{�I�ȃ����o�֐�
	void Initialize(DirectXBasis* dXBas);

	void Draw();

private: //�ŗL�̃����o�֐�
	//�C���X�^���X�̃|�C���^��ǂݍ���ŁA�����o�ϐ��ɃZ�b�g����
	void LoadInstance(DirectXBasis* dXBas);

	//���_�o�b�t�@�r���[�̍쐬
	void CreateVertexBufferView();
	//���_�f�[�^��g�ݗ��Ă�
	void AssembleVetices();

	//�V�F�[�_�t�@�C����ǂݍ��݁A�R���p�C������
	void CompileShaderFile();

	//���_���C�A�E�g��g�ݗ��Ă�
	void AssembleVertexLayout();

	//�O���t�B�b�N�X�p�C�v���C���𐶐�
	void CreateGraphicsPopeline();
	//�O���t�B�b�N�X�p�C�v���C���f�X�N�̒��g��ݒ�
	void SettingGraphicsPopelineDesc();
	//���[�g�V�O�l�`���𐶐�
	void CreateRootSignature();


	//�`�揀��
	void PrepareDraw();
	//�`��㏈��
	void PostDraw();

private:
	static const int ElementDescNum = 1;//inputLayout_�̃G�������g��
	static const int VerticesNum = 3;//vertices�̒��_��

	DirectXBasis* dXBas_ = nullptr;

	//���_�f�[�^
	/*Vector3 vertices_[VerticesNum];*/

	D3D12_VERTEX_BUFFER_VIEW vbView_{};//���_�o�b�t�@�r���[

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g

	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum];//���_���C�A�E�g

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//�O���t�B�b�N�X�p�C�v���C���f�X�N

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//���[�g�V�O�l�`��

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//�p�C�v���C���X�e�[�g
};