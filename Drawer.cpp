#include "Drawer.h"
#include <cassert>
#include <d3dcompiler.h>
#include <string>

void Drawer::Initialize(const wchar_t* vsFile,const wchar_t* psFile){
	
	LoadShaderFile(vsFile,psFile);
	
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			//xyz座標
			"POSITION",									//セマンティック名
			0,											//同じセマンティック名が複数あるときに使うインデックス(0でよい)
			DXGI_FORMAT_R32G32B32_FLOAT,				//要素数とビット数を表す　(XYZの3つでfloat型なのでR32G32B32_FLOAT
			0,											//入力スロットインデックス(0でよい)
			D3D12_APPEND_ALIGNED_ELEMENT,				//データのオフセット値　(D3D12_APPEND_ALIGNED_ELEMENTだと自動設定)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//入力データ種別　(標準はD3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
			0											//一度に描画するインスタンス数(0でよい)
		},//(1行で書いた方が見やすいかも)

		{
			//法線ベクトル
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},

		//座標以外に 色、テクスチャUIなどを渡す場合はさらに続ける
		{
			//UV座標
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0,
		},
		//{/*...*/},
	};

	//グラフィックスパイプライン設定
	SetingGraphicsPipeline();

	//定数バッファ生成
	CreateConstBuffer();
#pragma region constMapMaterial関連

#pragma endregion
}

//グラフィックスパイプライン設定
void Drawer::SetingGraphicsPipeline(){
	//シェーダーの設定
	SetingShader();

	//サンプルマスクの設定
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定

	//ラスタライザの設定
	SetingRasterizer();

	//アルファブレンディングの設定
	SetingAlphaBlend();

	//頂点レイアウトの設定
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc_.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//デプスステンシルステートの設定
	SetingDepthStencilState();

	//その他の設定
	SetingOther();
}


void Drawer::CreateConstBuffer(){
	CreateConstBufferMaterial();
}




//グラフィックスパイプライン設定の中身
void Drawer::LoadShaderFile(const wchar_t* vsFile,const wchar_t* psFile){
	HRESULT result;

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		vsFile,//シェーダファイル名
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
		psFile,//シェーダファイル名
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
void Drawer::SetingShader(){
	pipelineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();
}
void Drawer::SetingRasterizer(){
	//背面カリングも設定する
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面カリングする
	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc_.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に
}
void Drawer::SetingAlphaBlend(){
	//ブレンドステート
	pipelineDesc_.BlendState.RenderTarget[0].RenderTargetWriteMask 
		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB全てのチャネルを描画

	//レンダ―ターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc_.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//アルファ値共通設定
	blenddesc.BlendEnable = false; // ブレンド有効にする
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
	blenddesc.SrcBlend = D3D12_BLEND_ONE; //ソースの値をアルファ値
	blenddesc.DestBlend = D3D12_BLEND_ONE; //1.0f-ソースのアルファ値
}
void Drawer::SetingDepthStencilState(){
	pipelineDesc_.DepthStencilState.DepthEnable = true;
	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc_.DSVFormat = DXGI_FORMAT_R32_FLOAT;
}
void Drawer::SetingOther(){
	pipelineDesc_.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc_.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング
}


void Drawer::CreateConstBufferMaterial(){
	HRESULT result;

	//マテリアル定数バッファ
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbheapprop{};
	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
											  //リソース設定
	D3D12_RESOURCE_DESC cbresdesc{};
	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbresdesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256バイトアラインメント
	cbresdesc.Height = 1;
	cbresdesc.DepthOrArraySize = 1;
	cbresdesc.MipLevels = 1;
	cbresdesc.SampleDesc.Count = 1;
	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&cbheapprop, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); //マッピング

	// 値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); //RGBAで半透明の赤

	//マッピング解除
	constBuffMaterial->Unmap(0, nullptr);
	assert(SUCCEEDED(result));
}