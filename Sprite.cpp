#include "Sprite.h"
#include <DirectXMath.h>
#include <cassert>

using namespace DirectX;

void Sprite::Initialize(DrawBasis* drawBas){
	assert(drawBas);
	drawBas_ = drawBas;

	dXBas_ = drawBas_->GetDXBasis();
	vbView_ = drawBas_->GetVBView();
	pipelineState_ = drawBas_->GetPipelineState();
	rootSignature_ = drawBas_->GetRootSignature();
}

void Sprite::Draw(){
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//�`��R�}���h
	dXBas_->GetCommandList()->DrawInstanced(
		//_countof(vertices)
		VerticesNum, 1, 0, 0);
}

void Sprite::PrepareDraw() {
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g
}

void Sprite::PostDraw(){
	//dXBas_->GetCommandList() = nullptr;
}