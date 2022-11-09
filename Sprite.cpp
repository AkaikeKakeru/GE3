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
	//頂点バッファビューの設定コマンド
	dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//描画コマンド
	dXBas_->GetCommandList()->DrawInstanced(
		VerticesNum, 1, 0, 0);
}

void Sprite::PrepareDraw() {
}

void Sprite::PostDraw(){
}