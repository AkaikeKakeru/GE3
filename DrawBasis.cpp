#include <Windows.h>

#include "DrawBasis.h"
#include <DirectXMath.h>
#include "Vector3.h"
#include <wrl.h>

#include <d3d12.h>
#include <d3dcompiler.h>//シェーダ用コンパイラ

#include <string>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")//シェーダ用コンパイラ

using namespace DirectX;

void DrawBasis::Initialize(DirectXBasis* dXBas){
	LoadInstance(dXBas);

	CompileShaderFile();

	AssembleVertexLayout();

	CreateGraphicsPopeline();
}

void DrawBasis::Draw(){
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

void DrawBasis::LoadInstance(DirectXBasis* dXBas){
	dXBas_ = dXBas;
}

void DrawBasis::CreateVertexBufferView(){
	HRESULT result;

	typedef enum VerticesParts {
		LeftBottom,
		LeftTop,
		RightBottom,
	}VerticesParts;

	float left = -5.0f;
	float right = +5.0f;
	float top = +5.0f;
	float bottom = -5.0f;

	//頂点データ
	Vector3 vertices[VerticesNum];

	vertices[LeftBottom] = Vector3( left,bottom,0 );
	vertices[LeftTop] = Vector3( left,bottom,0 );
	vertices[RightBottom] = Vector3( left,bottom,0 );
	//AssembleVetices();

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));

	//頂点バッファの設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUの転送用
										   //リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応仮想メモリ(メインメモリ上)を取得
	Vector3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//全頂点に対して
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成
	/*D3D12_VERTEX_BUFFER_VIEW vbView{};*/
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点１つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(Vector3);
}

void DrawBasis::AssembleVetices(){
	typedef enum VerticesParts {
		LeftBottom,
		LeftTop,
		RightBottom,
	}VerticesParts;

	float left = -5.0f;
	float right = +5.0f;
	float top = +5.0f;
	float bottom = -5.0f;

	//頂点データ
	Vector3 vertices[VerticesNum];

	vertices[LeftBottom] = { left,bottom,0 };
	vertices[LeftTop] = { left,bottom,0 };
	vertices[RightBottom] = { left,bottom,0 };
}

void DrawBasis::CompileShaderFile(){
	HRESULT result;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "vs_5_0",//エントリーポイント名、シェーダ―モデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&vsBlob_, &errorBlob_);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",//シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//インクルード可能にする
		"main", "ps_5_0",//エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//デバッグ用設定
		0,
		&psBlob_, &errorBlob_);

	//エラーなら
	if (FAILED(result)) {
		//errorBlobからエラー内容をstring型にコピー
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//エラー内容を出力ウィンドウに表示
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void DrawBasis::AssembleVertexLayout(){
	typedef enum ElementName {
		Position,
	}ElementName;

	//頂点レイアウト
	inputLayout_[ElementName::Position] = {
		//xyz座標
		"POSITION",									//セマンティック名
		0,											//同じセマンティック名が複数あるときに使うインデックス(0でよい)
		DXGI_FORMAT_R32G32B32_FLOAT,				//要素数とビット数を表す　(XYZの3つでfloat型なのでR32G32B32_FLOAT
		0,											//入力スロットインデックス(0でよい)
		D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値　(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別　(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
		0											//一度に描画するインスタンス数(0でよい)
	};
}

void DrawBasis::CreateGraphicsPopeline(){
	HRESULT result;

	//グラフィックスパイプライン設定
	SettingGraphicsPopelineDesc();

	CreateRootSignature();

	//パイプラインステートの生成
	/*ComPtr<ID3D12PipelineState> pipelineState = nullptr;*/
	result = dXBas_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void DrawBasis::SettingGraphicsPopelineDesc(){
	//シェーダーの設定
	pipelineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザの設定
	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc_.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

	//ブレンドステート
	pipelineDesc_.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB全てのチャネルを描画

	//頂点レイアウトの設定
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout_;
	pipelineDesc_.InputLayout.NumElements = ElementDescNum;//_countof(inputLayout_);

	//図形の形状設定
	pipelineDesc_.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc_.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング

}

void DrawBasis::CreateRootSignature(){
	HRESULT result;

	//ルートシグネチャ
	//ComPtr<ID3D12RootSignature> rootSignature;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	//ルートシグネチャの生成
	result = dXBas_->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネイチャをセット
	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void DrawBasis::PrepareDraw(){

}

void DrawBasis::PostDraw(){

}