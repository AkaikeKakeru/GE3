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
	void LoadInstance(DirectXBasis* dXBas);
	void CreateVertexBufferView();
	void CompileShaderFile();
	void InputVertexLayout();
	void CreateGraphicsPopeline();

private:
	const int ElementDescNum = 1;//inputLayout_�̃G�������g��

	DirectXBasis* dXBas_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g

	D3D12_INPUT_ELEMENT_DESC inputLayout_[];
};