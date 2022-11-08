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
	//パイプラインステートとルートシグネチャの設定コマンド
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト

	//頂点バッファビューの設定コマンド
	dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//描画コマンド
	dXBas_->GetCommandList()->DrawInstanced(
		//_countof(vertices)
		VerticesNum, 1, 0, 0);
}

void Sprite::PrepareDraw() {
	//パイプラインステートとルートシグネチャの設定コマンド
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト
}

void Sprite::PostDraw(){
	//dXBas_->GetCommandList() = nullptr;
}