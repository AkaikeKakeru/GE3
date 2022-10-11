#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class  Drawer
{
public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://��{�I�ȃ����o�֐�
	void Initialize(const wchar_t* vsFile,const wchar_t* psFile);

private: //�ŗL�̃����o�ϐ�
	void LoadShaderFile(const wchar_t* vsFile,const wchar_t* psFile);

private: //�悭�g�������o�ϐ�
	////���_�V�F�[�_�[�t�@�C��
	//wchar_t* vsFile_;
	////�s�N�Z���V�F�[�_�[�t�@�C��
	//wchar_t* psFile_;

	//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> vsBlob_ = nullptr;
	//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;
	//�G���[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;

	//�O���t�B�b�N�X�p�C�v���C��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};
};