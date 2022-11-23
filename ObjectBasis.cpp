#include "ObjectBasis.h"
#include <d3dcompiler.h>
#include <string>
#include <cassert>

template <class Type>
using ComPtr = Microsoft::WRL::ComPtr<Type>;

//ComPtr<ID3D12Device> ObjectBasis::device_ = nullptr;
//ComPtr<ID3D12GraphicsCommandList> ObjectBasis::cmdList_ = nullptr;

ID3D12Device* ObjectBasis::device_ = nullptr;
ID3D12GraphicsCommandList* ObjectBasis::cmdList_ = nullptr;

std::vector<ObjectBasis::Vertex> ObjectBasis::vertices_;
std::vector<unsigned short> ObjectBasis::indices_;

void ObjectBasis::Initialize(/*ComPtr<ID3D12Device>*/
	ID3D12Device* device) {
	assert(device);
	ObjectBasis::device_ = device;

	HRESULT result;

#pragma region 描画情報初期設定
	//	float angle = 0.0f; //カメラの回転角

	//頂点データ
	Vertex vert[] = {
		//x		 y		z		法線	u	  v
		//前
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//左上
		{{ 5.0f, -5.0f, -5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 0.0f}},//右上

													  //後ろ				 
		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上
		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 1.0f}},//左下
		{{-5.0f,  5.0f,  5.0f},	{},		{0.0f, 0.0f}},//左上

													  //左							
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//左上
		{{-5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//右下
		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上

													  //右							
		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上
		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{ 5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//左上

													  //下							
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 0.0f}},//左上
		{{-5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上

													  //上							
		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 1.0f}},//右下
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//右上
		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 1.0f}},//左下
		{{ 5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//左上
	};

	//メンバ変数にコピー
	//std::copy(std::begin(vert), std::end(vert), vertices_);

	for (size_t i = 0; i < _countof(vert); i++) {
		vertices_.emplace_back(vert[i]);
	}

	//全頂点に対して
	//for (size_t i = 0; i < vertices_.size(); i++) {
	//	vertices_[i] = vert[i];//座標をコピー
	//}

	//インデックスデータ
	unsigned short indi[] = {
		//前
		0,1,2,//一つ目
		2,1,3,//二つ目
			  //後ろ
			  4,5,6,//三つ目
			  6,5,7,//四つ目
					//左
					8,9,10,//一つ目
					10,9,11,//二つ目
							//右
							12,13,14,
							14,13,15,
							//下
							16,17,18,//一つ目
							18,17,19,//二つ目
									 //上
									 20,21,22,
									 22,21,23,
	};

	//メンバ変数にコピー
	//std::copy(std::begin(indi), std::end(indi), indices_);
	for (size_t i = 0; i < _countof(indi); i++) {
		indices_.emplace_back(indi[i]);
	}
	//全頂点に対して
	//for (size_t i = 0; i < indices_.size(); i++) {
	//	indices_[i] = indi[i];//座標をコピー
	//}


	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	//UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices_.size());

	//頂点バッファの設定
	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUの転送用
	//リソース設定
	//D3D12_RESOURCE_DESC resDesc_{};
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;//頂点データ全体のサイズ
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = device_->CreateCommittedResource(
		&heapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	/* verticesに記入 */
	//GPU上のバッファに対応仮想メモリ(メインメモリ上)を取得
	//Vertex* vertMap_ = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap_);
	//if (SUCCEEDED(result)) {
	//}
	assert(SUCCEEDED(result));
		std::copy(vertices_.begin(), vertices_.end(), vertMap_);
		//繋がりを解除
		vertBuff->Unmap(0, nullptr);

	//全頂点に対して
	//for (size_t i = 0; i < vertices_.size(); i++) {
	//	vertMap_[i] = vertices_[i];//座標をコピー
	//}

	//頂点バッファビューの作成
	//D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点１つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);


	// インデックスデータ全体のサイズ
	//UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());


	// リソース設定
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeIB; // インデックス情報が入る分のサイズ
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	//ComPtr<ID3D12Resource> indexBuff_ = nullptr;
	result = device_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_));

	//　インデックスバッファをマッピング
	//unsigned short* indexMap_ = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap_);
	assert(SUCCEEDED(result));

		std::copy(indices_.begin(), indices_.end(), indexMap_);
		indexBuff_->Unmap(0, nullptr);

	// 全インデックスに対して
	//for (int i = 0; i < indices_.size(); i++) {
	//	indexMap_[i] = indices_[i]; //インデックスをコピー
	//}
	//マッピング解除

	//インデックスバッファビューの作成
	//D3D12_INDEX_BUFFER_VIEW ibView_{};
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;


	//ComPtr<ID3DBlob> vsBlob_ = nullptr;//頂点シェーダオブジェクト
	//ComPtr<ID3DBlob> psBlob_ = nullptr;//ピクセルシェーダオブジェクト
	//ComPtr<ID3DBlob> errorBlob_ = nullptr;//エラーオブジェクト

	//頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjectVS.hlsl",//シェーダファイル名
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
		L"Resources/shaders/ObjectPS.hlsl",//シェーダファイル名
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

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
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
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//シェーダーの設定
	pipelineDesc.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob_->GetBufferSize();

	//サンプルマスクの設定
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//標準設定
														//pipelineDesc.SampleMask = UINT_MAX;

														//ラスタライザの設定
														//背面カリングも設定する
														//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//背面カリングする
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//ポリゴン内塗りつぶし
	pipelineDesc.RasterizerState.DepthClipEnable = true;//深度クリッピングを有効に

														//ブレンドステート
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB全てのチャネルを描画

									   //レンダ―ターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
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

										   //頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状設定
	pipelineDesc.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

#pragma region 深度テストの設定
	//デプスステンシルステートの設定
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_R32_FLOAT;
#pragma endregion

	//その他の設定
	pipelineDesc.NumRenderTargets = 1;//描画対象は1つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0～255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;//1ピクセルにつき1回サンプリング

									  //ルートシグネチャ
									  //ComPtr<ID3D12RootSignature> rootSignature_;

									  //デスクリプタレンジの設定
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;//一度の描画に使うテクスチャが一枚なので1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//テクスチャレジスタ番号0番
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//デスクリプタテーブルの設定
	D3D12_DESCRIPTOR_RANGE descRange{};
	descRange.NumDescriptors = 1;//定数は一つ
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descRange.BaseShaderRegister = 0; //0番スロットから
	descRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	////ルートパラメータの設定
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//定数バッファ0番
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

																	//テクスチャレジスタ0番
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//定数バッファビュー
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;					//定数バッファ番号
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

																	//定数バッファ1番
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

																	//テクスチャサンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams; //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams); //ルートパラメータ数
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//ルートシグネチャのシリアライズ
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	result = device_->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネイチャをセット
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//パイプラインステートの生成
	//ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	result = device_->CreateGraphicsPipelineState(&pipelineDesc,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;

#pragma endregion

#pragma region constMapMaterial関連
	{

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
		result = device_->CreateCommittedResource(
			&cbheapprop, //ヒープ設定
			D3D12_HEAP_FLAG_NONE,
			&cbresdesc, //リソース設定
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffMaterial_));
		assert(SUCCEEDED(result));

		//定数バッファのマッピング
		//ConstBufferDataMaterial* constMapMaterial_ = nullptr;
		result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_); //マッピング

		// 値を書き込むと自動的に転送される
		constMapMaterial_->color = Vector4(0.5f, 0.5f, 0.5f, 1.0f); //RGBAで半透明の赤

		//マッピング解除
		constBuffMaterial_->Unmap(0, nullptr);
		assert(SUCCEEDED(result));

	}
#pragma endregion

#pragma region constMapTransfrom関連
	{

		//ヒープ設定
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
												  //リソース設定
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256バイトアラインメント
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

#pragma region 三次元オブジェクトの構造化

		//3Dオブジェクトの数
		//const size_t kObjectCount = 1;
		//3Dオブジェクトの配列
		//ObjectBasis object3ds[kObjectCount];

		Vector3 rndScale;
		Vector3 rndRota;
		Vector3 rndPos;

		//初期化
		//Initialize(/*dXBas_*/);

#pragma region constMapTransfrom関連
		{

			//ヒープ設定
			D3D12_HEAP_PROPERTIES heapProp{};
			heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
													//リソース設定
			//D3D12_RESOURCE_DESC resDesc{};
			resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resDesc_.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256バイトアラインメント
			resDesc_.Height = 1;
			resDesc_.DepthOrArraySize = 1;
			resDesc_.MipLevels = 1;
			resDesc_.SampleDesc.Count = 1;
			resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			//定数バッファの生成
			result = device_->CreateCommittedResource(
				&heapProp, //ヒープ設定
				D3D12_HEAP_FLAG_NONE,
				&resDesc_, //リソース設定
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&constBuffTransform_));
			assert(SUCCEEDED(result));

			//定数バッファのマッピング
			result = constBuffTransform_->Map(0, nullptr,
				(void**)&constMapTransform_); //マッピング
			assert(SUCCEEDED(result));
		}
#pragma endregion

#pragma endregion

#pragma region 単位行列で埋めた後
#pragma region 平行投影行列の計算

		////DirectXMathで用意されている関数に置き換え
		//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
		//	0.0f, window_width,//左端、右端
		//	window_height, 0.0f,//下端、上端
		//	0.0f, 1.0f);//前端、奥端
#pragma endregion

#pragma region ビュープロ使用
	////ビュープロジェクション初期化
	//viewPro_->Initialize();

	////配列内の全オブジェクトに対して
	//viewPro_->GetViewProjection() = viewPro_->GetViewProjection().matPro_;
	//viewPro_->view = viewPro_->GetViewProjection().matView_;
#pragma endregion

#pragma endregion


	//const int kTextureCount = 2;
	//TextureData textureDatas[kTextureCount] = { 0 };

		const wchar_t* texImgs = {
			L"Resources/smile.png"
		};

		InitializeTexture(texImgs);

		TransferTextureBuffer();

		//元データ開放
		//delete[] imageData;

		//SRVの最大個数
		const size_t kMaxSRVCount = 2056;

		//デスクリプタヒープの設定
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
		srvHeapDesc.NumDescriptors = kMaxSRVCount;

		//設定を基にSRV用デスクリプタヒープを生成
		//ID3D12DescriptorHeap* srvHeap_ = nullptr;
		result = device_->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
		assert(SUCCEEDED(result));

		//SRVヒープの先頭ハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();

		//シェーダリソースビュー設定
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//設定構造体
		srvDesc.Format = resDesc_.Format;//RGBA float
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		srvDesc.Texture2D.MipLevels = resDesc_.MipLevels;

		//ハンドルの指す位置にシェーダーリソースビュー作成
		device_->CreateShaderResourceView(textureData_.texBuff.Get(), &srvDesc, srvHandle);

#pragma region テクスチャの差し替えで追記
		//サイズ変更
		UINT incrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		srvHandle.ptr += incrementSize;

		////2枚目用
		////シェーダリソースビュー設定
		//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//設定構造体
		//srvDesc2.Format = textureDatas[1].textureResourceDesc.Format;//RGBA float
		//srvDesc2.Shader4ComponentMapping =
		//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
		//srvDesc2.Texture2D.MipLevels = textureDatas[1].textureResourceDesc.MipLevels;

		////ハンドルの指す位置にシェーダーリソースビュー作成
		//device_->CreateShaderResourceView(textureDatas[1].texBuff.Get(), &srvDesc2, srvHandle);

#pragma endregion

	//CBV,SRV,UAVの1個分のサイズを取得
		UINT descriptorSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//SRVヒープの先頭ハンドルを取得
		//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
		//ハンドルを一つ進める(SRVの位置)
		srvHandle.ptr += descriptorSize * 1;

		//CBV(コンスタントバッファビュー)の設定
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		//cbvDescの値設定(省略)
		device_->CreateConstantBufferView(&cbvDesc, srvHandle);

	}
#pragma endregion

#pragma endregion
}

void ObjectBasis::InitializeTexture(const wchar_t* szFile) {
	HRESULT result;

	//WICテクスチャのロード
	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&textureData_.metadata,
		textureData_.scratchImg);

	//ミップマップ生成
	result = GenerateMipMaps(
		textureData_.scratchImg.GetImages(),
		textureData_.scratchImg.GetImageCount(),
		textureData_.scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, textureData_.mipChine);
	if (SUCCEEDED(result)) {
		textureData_.scratchImg = std::move(textureData_.mipChine);
		textureData_.metadata = textureData_.scratchImg.GetMetadata();
	}
	//読み込んだでデイヒューズテクスチャをSRGBとして扱う
	textureData_.metadata.format = MakeSRGB(textureData_.metadata.format);
};

void ObjectBasis::TransferTextureBuffer() {
	HRESULT result;

	//ヒープ設定
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureData_.textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData_.textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData_.textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	textureData_.textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData_.textureResourceDesc.Format = textureData_.metadata.format;
	textureData_.textureResourceDesc.Width = textureData_.metadata.width; //幅
	textureData_.textureResourceDesc.Height = (UINT)textureData_.metadata.height; //高さ
	textureData_.textureResourceDesc.DepthOrArraySize = (UINT16)textureData_.metadata.arraySize;
	textureData_.textureResourceDesc.MipLevels = (UINT16)textureData_.metadata.mipLevels;
	textureData_.textureResourceDesc.SampleDesc.Count = 1;

	////テクスチャバッファの生成
	//ID3D12Resource* texBuff = nullptr;

	result = device_->CreateCommittedResource(
		&textureData_.textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData_.textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData_.texBuff));

	//全ミップマップについて
	for (size_t i = 0; i < textureData_.metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = textureData_.scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureData_.texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//一枚サイズ
		);
		assert(SUCCEEDED(result));
	}
}

//オブジェクト更新処理
void  ObjectBasis::Update() {
	//Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	////スケール、回転、平行移動行列の計算
	//matScale = Mat4Scale(worldTransform_.scale_);
	//matRota = Mat4Identity();

	//matZ = Mat4RotationZ(worldTransform_.rotation_.z);
	//matX = Mat4RotationX(worldTransform_.rotation_.x);
	//matY = Mat4RotationY(worldTransform_.rotation_.y);

	//matRota = matRota * matZ * matX * matY;

	//matTrans = Mat4Translation(worldTransform_.position_);

	////ワールド行列の合成
	//worldTransform_.matWorld_ = Mat4Identity();	//変形リセット

	//worldTransform_.matWorld_ *= matScale;	//ワールド行列のスケーリングを反映
	//worldTransform_.matWorld_ *= matRota;	//ワールド行列に回転を反映
	//worldTransform_.matWorld_ *= matTrans;	//ワールド行列に平行移動を反映

	worldTransform_.RecalculationMatWorld();

	//親オブジェクトがあれば
	if (parent_ != nullptr) {
		//親オブジェクトのワールド行列を掛ける
		worldTransform_.matWorld_ *= parent_->worldTransform_.matWorld_;
	}

	//定数バッファへデータ転送
	constMapTransform_->mat = worldTransform_.matWorld_ *
		viewPro_->GetViewProjection().matView_ *
		viewPro_->GetViewProjection().matPro_;
}

void  ObjectBasis::Draw(/*UINT numIndices*/) {
	////頂点バッファの設定
	//dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);
	////インデックスバッファの設定
	//dXBas_->GetCommandList()->IASetIndexBuffer(&ibView_);
	////定数バッファビュー(CBV)の設定コマンド
	//dXBas_->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	////描画コマンド
	//dXBas_->GetCommandList()->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);

	assert(device_);
	assert(ObjectBasis::cmdList_);

	//頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	//インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView_);

	//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//定数バッファビュー(CBV)の設定コマンド
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());
	//SRVヒープの設定コマンド
	cmdList_->SetDescriptorHeaps(1, &srvHeap_);

	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//描画コマンド
	cmdList_->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);


}

void ObjectBasis::copyInit(DirectXBasis* dXBas) {

}

void  ObjectBasis::copyUp() {
}

void ObjectBasis::copyDraw() {

#pragma region 法線を計算
	for (int i = 0; i < indices_.size() / 3; i++) {
		//三角形１つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = indices_[static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(i) * 3 + 0];
		unsigned short index1 = indices_[static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(i) * 3 + 1];
		unsigned short index2 = indices_[static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(i) * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		Vector3 p0 = vertices_[index0].pos;
		Vector3 p1 = vertices_[index1].pos;
		Vector3 p2 = vertices_[index2].pos;

		//p0->p1ベクトル、 p0->p2ベクトルを計算 (ベクトルの減算)
		Vector3 v1 = p1 - p0;
		Vector3 v2 = p2 - p0;

		//外積は両方から垂直なベクトル
		Vector3 normal = Vec3Cross(v1, v2);

		//正規化 (長さを1にする)
		normal = Vec3Normalize(normal);

		//求めた法線を頂点データに代入
		vertices_[index0].normal = normal;
		vertices_[index1].normal = normal;
		vertices_[index2].normal = normal;
	}
#pragma endregion

#pragma region 転送
	//全頂点に対して
	for (int i = 0; i < vertices_.size(); i++) {
		vertMap_[i] = vertices_[i];//座標をコピー
	}

	//result = vertBuff->Map(0, nullptr, (void**)&vertMap_);
	//assert(SUCCEEDED(result));
	//std::copy(vertices_.begin(), vertices_.end(), vertMap_);
#pragma endregion
}

void ObjectBasis::PrepareDraw(/*ComPtr<ID3D12GraphicsCommandList>*/
	ID3D12GraphicsCommandList* cmdList) {
	/*assert(dXBas_->GetCommandList());*/
	assert(ObjectBasis::cmdList_ == nullptr);

	ObjectBasis::cmdList_ = cmdList;

	//パイプラインステートとルートシグネチャの設定コマンド
	cmdList->SetPipelineState(pipelineState_.Get());
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト
}

void ObjectBasis::PostDraw() {
	//コマンドリスト解除
	ObjectBasis::cmdList_ = nullptr;
}