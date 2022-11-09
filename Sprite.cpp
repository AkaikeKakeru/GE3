#include "Sprite.h"
#include <cassert>

void Sprite::Initialize(DrawBasis* drawBas){
	assert(drawBas);
	drawBas_ = drawBas;

	dXBas_ = drawBas_->GetDXBasis();
	vbView_ = drawBas_->GetVBView();
	pipelineState_ = drawBas_->GetPipelineState();
	rootSignature_ = drawBas_->GetRootSignature();
}

void Sprite::Draw(){
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//�`��R�}���h
	dXBas_->GetCommandList()->DrawInstanced(
		VerticesNum, 1, 0, 0);
}

void Sprite::PrepareDraw() {
}

void Sprite::PostDraw(){
}