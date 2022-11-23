#include "Sprite.h"
#include <cassert>

template <class Type>
using ComPtr = Microsoft::WRL::ComPtr<Type>;

ID3D12GraphicsCommandList* Sprite::cmdList_ = nullptr;

void Sprite::Initialize(DrawBasis* drawBas){
	assert(drawBas);
	drawBas_ = drawBas;

	vbView_ = drawBas_->GetVBView();
	pipelineState_ = drawBas_->GetPipelineState();
	rootSignature_ = drawBas_->GetRootSignature();
}

void Sprite::Draw(){
	PrepareDraw();

	//頂点バッファビューの設定コマンド
	drawBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//描画コマンド
	drawBas_->GetCommandList()->DrawInstanced(
		VerticesNum, 1, 0, 0);

	PostDraw();
}

void Sprite::PrepareDraw() {
	cmdList_ = drawBas_->GetCommandList();
}

void Sprite::PostDraw(){
	Sprite::cmdList_ = nullptr;
}