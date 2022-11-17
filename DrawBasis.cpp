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
	/*アスペクト比を考慮したうえで正方形っぽくする*/
	float left = -0.4f;//左
	float right = +0.4f;//右
	float top = +0.7f;//上
	float bottom = -0.7f;//下

	/*初期タイプ*/
	//float left = -0.5f;//左
	//float right = +0.5f;//右
	//float top = +0.5f;//上
	//float bottom = -0.5f;//下

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
	vertices[RightBottom].uv = Vector2(uvRight, uvBottom);
	vertices[RightTop].uv = Vector2(uvRight, uvTop);

	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//頂点バッファの設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUの転送用
	//リソース設定
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;//頂点データ全体のサイズ
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	result = dXBas->GetDevice()->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応仮想メモリ(メインメモリ上)を取得
	Vertex* vertMap = nullptr;
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
		Position,//xyz座標
		Texcoord,//uv座標
	}ElementName;

	//頂点レイアウト
	for (int i = 0; i < ElementDescNum; i++) {
		switch (i) {
		case ElementName::Position:
			inputLayout_[i] = {
				//xyz座標
				"POSITION",									//セマンティック名
				0,											//同じセマンティック名が複数あるときに使うインデックス(0でよい)
				DXGI_FORMAT_R32G32B32_FLOAT,				//要素数とビット数を表す　(XYZの3つでfloat型なのでR32G32B32_FLOAT
				0,											//入力スロットインデックス(0でよい)
				D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値　(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別　(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
				0											//一度に描画するインスタンス数(0でよい)
			};
			break;

		case ElementName::Texcoord:
			inputLayout_[i] = {
				//uv座標
				"TEXCOORD",									//セマンティック名
				0,											//同じセマンティック名が複数あるときに使うインデックス(0でよい)
				DXGI_FORMAT_R32G32_FLOAT,				//要素数とビット数を表す　(XYZの3つでfloat型なのでR32G32B32_FLOAT
				0,											//入力スロットインデックス(0でよい)
				D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値　(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別　(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
				0											//一度に描画するインスタンス数(0でよい)
			};
			break;

		default:
			break;
		}
	}
}

void DrawBasis::CreateGraphicsPipeline(DirectXBasis* dXBas) {
	HRESULT result;

	//グラフィックスパイプラインデスクの中身を設定
	SettingGraphicsPipelineDesc();

	//デスクリプタレンジ設定
	SettingDescriptorRange();

	//ルートパラメータ設定
	SettingRootParameter();

	//テクスチャサンプラー設定
	SettingTextureSampler();

	//ルートシグネチャを生成
	CreateRootSignature(dXBas_);

	//パイプラインステートの生成
	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//定数バッファ生成
	CreateConstBuffer();

	//テクスチャ初期化
	initializeTexture();

	//デスクリプタヒープ生成
	CreateDescriptorHeap();

	//シェーダリソースビュー生成
	CreateShagerResourceView();
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
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-デストから－の値
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
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
}

void DrawBasis::SettingRootParameter() {
	//ルートパラメータの設定
	//定数バッファ0番
	rootParams_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams_[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams_[0].Descriptor.RegisterSpace = 0;						//デフォルト
	rootParams_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
	//テクスチャレジスタ0番
	rootParams_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//デスクリプタテーブル
	rootParams_[1].DescriptorTable.pDescriptorRanges = &descriptorRange_;				//デスクリプタレンジ
	rootParams_[1].DescriptorTable.NumDescriptorRanges = 1;					//デスクリプタレンジ数
	rootParams_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
}

void DrawBasis::CreateRootSignature(DirectXBasis* dXBas) {
	HRESULT result;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	/*ルートパラメータ情報*/
	rootSignatureDesc.pParameters = rootParams_;//ルートパラメータ先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams_);//ルートパラメータ数
	/*テクスチャサンプラー情報*/
	rootSignatureDesc.pStaticSamplers = &samplerDesc_;
	rootSignatureDesc.NumStaticSamplers = 1;

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

void DrawBasis::initializeTexture() {
	HRESULT result;

	/////初期化
	//imageData_ = new Vector4[imageDataCount]; //※必ず開放する

	////全ピクセルの色を初期化
	//for (size_t i = 0; i < imageDataCount; i++) {
	//	imageData_[i].x = 1.0f;	//R
	//	imageData_[i].y = 0.0f;	//G
	//	imageData_[i].z = 0.0f;	//B
	//	imageData_[i].w = 1.0f;	//A
	//}

	//WICテクスチャのロード
	result = LoadFromWICFile(
		L"Resources/smile.png",
		WIC_FLAGS_NONE,
		&metadata_, scratchImg_);

	//ミップマップ生成
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg_.GetImages(), scratchImg_.GetImageCount(), scratchImg_.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg_ = std::move(mipChain);
		metadata_ = scratchImg_.GetMetadata();
	}

	//読み込んだディフューズテクスチャをSRGBとして扱う
	metadata_.format = MakeSRGB(metadata_.format);

	///テクスチャバッファ
	//テクスチャバッファ生成
	CreateTextureBuffer();
	//テクスチャバッファ転送
	TransferTextureBuffer();

	//元データ解放
	delete[] imageData_;
	imageData_ = nullptr;
}

void DrawBasis::CreateTextureBuffer() {
	HRESULT result;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata_.format;
	textureResourceDesc.Width = metadata_.width; //幅
	textureResourceDesc.Height = (UINT)metadata_.height; //高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata_.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata_.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	result = dXBas_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff_));
}

void DrawBasis::TransferTextureBuffer() {
	HRESULT result;

	//テクスチャバッファにデータ転送
	result = texBuff_->WriteToSubresource(
		0,
		nullptr,//全領域へコピー
		imageData_,//元データアドレス
		static_cast<UINT>(sizeof(Vector4) * textureWidth),//1ラインサイズ
		static_cast<UINT>(sizeof(Vector4) * imageDataCount)//一枚サイズ
	);
	assert(SUCCEEDED(result));
}

void DrawBasis::CreateDescriptorHeap() {
	HRESULT result;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc.NumDescriptors = static_cast<UINT>(kMaxSRVCount);

	//設定を基にSRV用デスクリプタヒープを生成
	result = dXBas_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	//SRVヒープの先頭ハンドルを取得
	srvHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();
}

void DrawBasis::CreateShagerResourceView() {
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
	srvDesc.Format = resDesc_.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = resDesc_.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	dXBas_->GetDevice()->CreateShaderResourceView(texBuff_.Get(), &srvDesc, srvHandle_);
}

void DrawBasis::SettingDescriptorRange() {
	//デスクリプタレンジの設定
	descriptorRange_.NumDescriptors = 1;//一度の描画に使うテクスチャが一枚なので1
	descriptorRange_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_.BaseShaderRegister = 0;//テクスチャレジスタ番号0番
	descriptorRange_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void DrawBasis::SettingTextureSampler(){
	//テクスチャサンプラーの設定
	samplerDesc_.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc_.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc_.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc_.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc_.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc_.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc_.MinLOD = 0.0f;
	samplerDesc_.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc_.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
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

	//SRVヒープの設定コマンド
	dXBas_->GetCommandList()->SetDescriptorHeaps(1, &srvHeap_);

	//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();

	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	dXBas_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
}

void DrawBasis::PostDraw() {

}