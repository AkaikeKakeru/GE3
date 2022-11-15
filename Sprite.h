#pragma once
#include "DrawBasis.h"

class Sprite{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //基本的なメンバ関数
	void Initialize(DrawBasis* drawBas);
	void Draw();

	//描画準備
	void PrepareDraw();
	//描画後処理
	void PostDraw();

private: //メンバ変数
	static const int VerticesNum = 4;//verticesの頂点数

	DirectXBasis* dXBas_ = nullptr; //DirectX基盤

	DrawBasis* drawBas_ = nullptr; //描画基盤

	D3D12_VERTEX_BUFFER_VIEW vbView_{};//頂点バッファビュー

	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//ルートシグネチャ

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//パイプラインステート
};