#include <Windows.h>

#include "DrawBasis.h"
#include <DirectXMath.h>
#include "Vector3.h"
#include "Vector2.h"
#include <wrl.h>

#include <d3d12.h>
#include <d3dcompiler.h>//シェーダ用コンパイラ

#include <string>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")//シェーダ用コンパイラ

using namespace DirectX;

void DrawBasis::Initialize(DirectXBasis* dXBas) {
	assert(dXBas);
	dXBas_ = dXBas;

	//頂点バッファビューの作成
	CreateVertexBufferView(dXBas_);

	//シェーダファイルを読み込み、コンパイルする
	CompileShaderFile();

	//頂点レイアウトを組み立てる
	AssembleVertexLayout();

	//グラフィックスパイプラインを生成
	CreateGraphicsPipeline(dXBas_);
}

void DrawBasis::CreateVertexBufferView(DirectXBasis* dXBas) {
	HRESULT result;

	//頂点データ構造体
	struct Vertex {
		Vector3 pos;//xyz座標
		Vector2 uv;//uv座標
	};

	//頂点データ
	Vertex vertices[VerticesNum];

	//頂点部位
	typedef enum VerticesParts {
		LeftBottom,//左下
		LeftTop,//左上
		RightBottom,//右下
		RightTop,//右上
	}VerticesParts;

	//各初期位置関係
	float left = -0.5f;//左
	float right = +0.5f;//右
	float top = +0.5f;//上
	float bottom = -0.5f;//下

	float uvLeft = 0.0f;//uv左
	float uvRight = 1.0f;//uv右
	float uvTop = 0.0f;//uv上
	float uvBottom = 1.0f;//uv下

	//各部位に、初期位置関係を設定
	vertices[LeftBottom].pos = Vector3(left, bottom, 0);
	vertices[LeftTop].pos = Vector3(left, top, 0);
	vertices[RightBottom].pos = Vector3(right, bottom, 0);
	vertices[RightTop].pos = Vector3(right, top, 0);

	vertices[LeftBottom].uv = Vector2(uvLeft, uvBottom);
	vertices[LeftTop].uv = Vector2(uvLeft, uvTop);
	vertices[RightBottom].uv = Vector2(uvRight,uvBottom);
	vertices[RightTop].uv = Vector2(uvRight, uvTop);

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

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
	result = dXBas->GetDevice()->CreateCommittedResource(
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
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点１つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices[0]);
}

void DrawBasis::CompileShaderFile() {
	HRESULT result;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",//シェーダファイル名
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
		L"Resources/shaders/SpritePS.hlsl",//シェーダファイル名
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

void DrawBasis::AssembleVertexLayout() {
	//要素名
	typedef enum ElementName {
		Position,//座標
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

void DrawBasis::CreateGraphicsPipeline(DirectXBasis* dXBas) {
	HRESULT result;

	//グラフィックスパイプラインデスクの中身を設定
	SettingGraphicsPipelineDesc();

	//ルートパラメータ設定
	SettingRootParameter();

	//ルートシグネチャを生成
	CreateRootSignature(dXBas_);

	//パイプラインステートの生成
	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//定数バッファ生成
	CreateConstBuffer();
}

void DrawBasis::SettingGraphicsPipelineDesc() {
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
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc_.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGB全てのチャネルを描画

	//アルファ値共通設定
	blenddesc.BlendEnable = true; // ブレンド有効にする
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //ブレンドを有効にする
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; //加算
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //デストの値を 0%使う　

	//加算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE; //デストの値を100%使う

	//減算合成
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //減算
	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値を100%使う
	//blenddesc.DestBlend = D3D12_BLEND_ONE; //デストの値を100%使う

	//色反転
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-デストから−の値
	//blenddesc.DestBlend = D3D12_BLEND_ZERO; //使わない

	//半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //加算
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //ソースの値をアルファ値
	blenddesc.DestBlend = D3D12_BLEND_SRC_ALPHA; //1.0f-ソースのアルファ値


//頂点レイアウトの設定
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout_;
	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout_);

	//図形の形状設定
	pipelineDesc_.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	pipelineDesc_.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0〜255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
}

void DrawBasis::SettingRootParameter() {
	//ルートパラメータの設定
	rootParam_.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParam_.Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParam_.Descriptor.RegisterSpace = 0;						//デフォルト
	rootParam_.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
}

void DrawBasis::CreateRootSignature(DirectXBasis* dXBas) {
	HRESULT result;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	/*ルートパラメータ情報*/
	rootSignatureDesc.pParameters = &rootParam_;//ルートパラメータ先頭アドレス
	rootSignatureDesc.NumParameters = 1;//ルートパラメータ数

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	//ルートシグネチャの生成
	result = dXBas->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネイチャをセット
	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void DrawBasis::CreateConstBuffer() {
	HRESULT result;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
											  //リソース設定
	D3D12_RESOURCE_DESC cbResDesc{};
	cbResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256バイトアラインメント
	cbResDesc.Height = 1;
	cbResDesc.DepthOrArraySize = 1;
	cbResDesc.MipLevels = 1;
	cbResDesc.SampleDesc.Count = 1;
	cbResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&cbHeapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffMaterial_->Map(0, nullptr,
		(void**)&constMapMaterial_); //マッピング
	assert(SUCCEEDED(result));

	//値を書き込むと自動的に転送される
	constMapMaterial_->color = Vector4(1, 0, 0, 0.5f);
}

void DrawBasis::PrepareDraw() {
	//パイプラインステートとルートシグネチャの設定コマンド
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	//dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//三角形リスト

	// 定数バッファビュー(CBV)の設定コマンド
	dXBas_->GetCommandList()->SetGraphicsRootConstantBufferView(
		0, constBuffMaterial_->GetGPUVirtualAddress());
}

void DrawBasis::PostDraw() {

}