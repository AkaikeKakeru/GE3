#pragma once
#include "DrawBasis.h"

class Sprite{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //��{�I�ȃ����o�֐�
	void Initialize(DrawBasis* drawBas);
	void Draw();

	//�`�揀��
	void PrepareDraw();
	//�`��㏈��
	void PostDraw();

private: //�����o�ϐ�
	static const int VerticesNum = 4;//vertices�̒��_��

	DirectXBasis* dXBas_ = nullptr; //DirectX���

	DrawBasis* drawBas_ = nullptr; //�`����

	D3D12_VERTEX_BUFFER_VIEW vbView_{};//���_�o�b�t�@�r���[

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//���[�g�V�O�l�`��

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//�p�C�v���C���X�e�[�g
};