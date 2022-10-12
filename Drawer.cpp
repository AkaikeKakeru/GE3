#include "Drawer.h"
#include <cassert>
#include <d3dcompiler.h>
#include <string>

void Drawer::Initialize(DirectXBasis* dXBas,
	const wchar_t* vsFile,const wchar_t* psFile){
	assert(dXBas);
	dXBas_ = dXBas;

	LoadShaderFile(vsFile,psFile);

	VertexBufferInitialize();

	//グラフィックスパイプライン設定
	SetingGraphicsPipeline();

	//定数バッファ生成
	CreateConstBuffer();
#pragma region constMapMaterial関連

#pragma endregion

	TextureInitialize();
}

void Drawer::VertexBufferInitialize(){
	HRESULT result;
	
	//頂点データ
	vertices_ =
	{
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


	//頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * /*_countof(*/vertices_.size());

	//頂点バッファの設定
	//ヒープ設定
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;//GPUの転送用
	//リソース設定
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;//頂点データ全体のサイズ
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//頂点バッファの生成
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&heapProp_,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,//リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU上のバッファに対応仮想メモリ(メインメモリ上)を取得
	
	result = vertBuff->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));

	/* verticesに記入 */

	//全頂点に対して
	for (int i = 0; i < vertices_.size(); i++)
	{
		vertMap_[i] = vertices_[i];//座標をコピー
	}

	//繋がりを解除
	vertBuff->Unmap(0, nullptr);

	//頂点バッファビューの作成;
	//GPU仮想アドレス
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	//頂点１つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);

}

void Drawer::TextureInitialize(){
	HRESULT result;

	//インデックスデータ
	indices_ =
	{
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


	// インデックスデータ全体のサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * /*_countof(*/indices_.size())/*)*/;

	// リソース設定
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeIB; // インデックス情報が入る分のサイズ
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//インデックスバッファの生成
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&heapProp_,
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//　インデックスバッファをマッピング
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	// 全インデックスに対して
	for (int i = 0; i < /*_countof(*/indices_.size()/*)*/; i++)
	{
		indexMap[i] = indices_[i]; //インデックスをコピー
	}
	//マッピング解除
	indexBuff->Unmap(0, nullptr);

	//インデックスバッファビューの作成
	
	ibView_.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	const int kTextureCount = 2;
	TextureData textureDatas[kTextureCount] = { 0 };

	const wchar_t* texImgs[kTextureCount] =
	{
		L"Resources/texture.png",
		L"Resources/reimu.png",
	};

	for (size_t i = 0; i < _countof(textureDatas); i++)
	{
		InitializeTexture(&textureDatas[i], texImgs[i]);
	}

	for (size_t i = 0; i < _countof(textureDatas); i++)
	{
		TransferTextureBuffer(&textureDatas[i], dXBas_->GetDevice());
	}

	//SRVの最大個数
	const size_t kMaxSRVCount = 2056;

	//デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc_ = {};
	srvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
	srvHeapDesc_.NumDescriptors = kMaxSRVCount;

	//設定を基にSRV用デスクリプタヒープを生成
	result = dXBas_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc_, IID_PPV_ARGS(&srvHeap_));
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
	dXBas_->GetDevice()->CreateShaderResourceView(textureDatas[0].texBuff.Get(), &srvDesc, srvHandle);

#pragma region テクスチャの差し替えで追記
	//サイズ変更
	incrementSize_ = dXBas_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize_;

	//2枚目用
	//シェーダリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//設定構造体
	srvDesc2.Format = textureDatas[1].textureResourceDesc.Format;//RGBA float
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = textureDatas[1].textureResourceDesc.MipLevels;

	//ハンドルの指す位置にシェーダーリソースビュー作成
	dXBas_->GetDevice()->CreateShaderResourceView(textureDatas[1].texBuff.Get(), &srvDesc2, srvHandle);

#pragma endregion

	//CBV,SRV,UAVの1個分のサイズを取得
	UINT descriptorSize = dXBas_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVヒープの先頭ハンドルを取得
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//ハンドルを一つ進める(SRVの位置)
	srvHandle.ptr += descriptorSize * 1;

	//CBV(コンスタントバッファビュー)の設定
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	//cbvDescの値設定(省略)
	dXBas_->GetDevice()->CreateConstantBufferView(&cbvDesc, srvHandle);

#pragma endregion

#pragma endregion
}

void Drawer::Update(){
	//パイプラインステートとルートシグネチャの設定コマンド
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//プリミティブ形状の設定コマンド
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//三角形リスト

																						 //頂点バッファビューの設定コマンド
	dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//定数バッファビュー(CBV)の設定コマンド
	dXBas_->GetCommandList()->SetGraphicsRootConstantBufferView(0,constBuffMaterial_->GetGPUVirtualAddress());

	//SRVヒープの設定コマンド
	dXBas_->GetCommandList()->SetDescriptorHeaps(1, &srvHeap_);

	//SRVヒープの先頭ハンドルを取得(SRVを指しているはず)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	//SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	dXBas_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	if (ifOneTextureNum_ == false)
	{
		//2枚目を指し示すようにしたSRVのハンドルをルートパラメータに設定
		srvGpuHandle.ptr += incrementSize_;
		dXBas_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}

	//インデックスバッファビューの設定コマンド
	dXBas_->GetCommandList()->IASetIndexBuffer(&ibView_);



}

//グラフィックスパイプライン設定
void Drawer::SetingGraphicsPipeline(){
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

	//デスクリプタレンジの設定
	SetingDescriptorRange();
	//デスクリプタテーブルの設定
	SetingDescriptorTable();

	//テクスチャサンプラーの設定
	SetingTextureSampler();

	///ルートシグネチャ関連
	//ルートパラメータの設定
	SetingRootParameter();
	//ルートシグネチャの設定
	SetingRootSignature();

	//パイプラインステート生成
	CreatePipelineState();
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

void Drawer::SetingDescriptorRange(){
	descriptorRange_.NumDescriptors = 1;//一度の描画に使うテクスチャが一枚なので1
	descriptorRange_.RangeType = 
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_.BaseShaderRegister = 0;//テクスチャレジスタ番号0番
	descriptorRange_.OffsetInDescriptorsFromTableStart = 
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void Drawer::SetingDescriptorTable(){
	descRange_.NumDescriptors = 1;//定数は一つ
	descRange_.RangeType = 
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //種別は定数
	descRange_.BaseShaderRegister = 0; //0番スロットから
	descRange_.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void Drawer::SetingTextureSampler(){
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

void Drawer::SetingRootParameter(){
	///ルートパラメータの設定
	
	//定数バッファ0番
	rootParams_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams_[0].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams_[0].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

	//テクスチャレジスタ0番
	rootParams_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//定数バッファビュー
	rootParams_[1].DescriptorTable.pDescriptorRanges = &descriptorRange_;					//定数バッファ番号
	rootParams_[1].DescriptorTable.NumDescriptorRanges = 1;						//デフォルト値
	rootParams_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える

	//定数バッファ1番
	rootParams_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams_[2].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams_[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//全てのシェーダから見える
}
void Drawer::SetingRootSignature(){
	HRESULT result;

	//ルートシグネチャの設定
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams_; //ルートパラメータの先頭アドレス
	rootSignatureDesc.NumParameters = _countof(rootParams_); //ルートパラメータ数
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

	result = dXBas_->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//パイプラインにルートシグネイチャをセット
	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void Drawer::CreatePipelineState(){
	HRESULT result;

	//パイプラインステートの生成
	result = dXBas_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void Drawer::CreateConstBufferMaterial(){
	HRESULT result;

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
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial); //マッピング

	// 値を書き込むと自動的に転送される
	constMapMaterial->color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); //RGBAで半透明の赤

	//マッピング解除
	constBuffMaterial_->Unmap(0, nullptr);
	assert(SUCCEEDED(result));
}

void Drawer::TransferVertices(){
#pragma region 法線を計算
	for (int i = 0; i < indices_.size() / 3; i++)
	{
		//三角形１つごとに計算していく
		//三角形のインデックスを取り出して、一時的な変数に入れる
		unsigned short index0 = indices_[i * 3 + 0];
		unsigned short index1 = indices_[i * 3 + 1];
		unsigned short index2 = indices_[i * 3 + 2];

		//三角形を構成する頂点座標をベクトルに代入
		XMVECTOR p0 = XMLoadFloat3(&vertices_[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices_[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices_[index2].pos);

		//p0->p1ベクトル、 p0->p2ベクトルを計算 (ベクトルの減算)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//外積は両方から垂直なベクトル
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//正規化 (長さを1にする)
		normal = XMVector3Normalize(normal);

		//求めた法線を頂点データに代入
		XMStoreFloat3(&vertices_[index0].normal, normal);
		XMStoreFloat3(&vertices_[index1].normal, normal);
		XMStoreFloat3(&vertices_[index2].normal, normal);
	}
#pragma endregion

#pragma region 転送
	//全頂点に対して
	for (int i = 0; i < vertices_.size(); i++)
	{
		vertMap_[i] = vertices_[i];//座標をコピー
	}
#pragma endregion
}

void Drawer::InitializeTexture(TextureData* textureData, const wchar_t* szFile)
{
	HRESULT result;

	//WICテクスチャのロード
	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&textureData->metadata,
		textureData->scratchImg);

	//ミップマップ生成
	result = GenerateMipMaps(
		textureData->scratchImg.GetImages(),
		textureData->scratchImg.GetImageCount(),
		textureData->scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, textureData->mipChine);
	if (SUCCEEDED(result)) {
		textureData->scratchImg = std::move(textureData->mipChine);
		textureData->metadata = textureData->scratchImg.GetMetadata();
	}
	//読み込んだでデイヒューズテクスチャをSRGBとして扱う
	textureData->metadata.format = MakeSRGB(textureData->metadata.format);
}

void Drawer::TransferTextureBuffer(TextureData* textureData, ID3D12Device* device)
{
	HRESULT result;

	//ヒープ設定
	textureData->textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData->textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData->textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	textureData->textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData->textureResourceDesc.Format = textureData->metadata.format;
	textureData->textureResourceDesc.Width = textureData->metadata.width; //幅
	textureData->textureResourceDesc.Height = (UINT)textureData->metadata.height; //高さ
	textureData->textureResourceDesc.DepthOrArraySize = (UINT16)textureData->metadata.arraySize;
	textureData->textureResourceDesc.MipLevels = (UINT16)textureData->metadata.mipLevels;
	textureData->textureResourceDesc.SampleDesc.Count = 1;

	////テクスチャバッファの生成
	result = device->CreateCommittedResource(
		&textureData->textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData->textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData->texBuff));

	//全ミップマップについて
	for (size_t i = 0; i < textureData->metadata.mipLevels; i++)
	{
		//ミップマップレベルを指定してイメージを取得
		const Image* img = textureData->scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureData->texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//全領域へコピー
			img->pixels,//元データアドレス
			(UINT)img->rowPitch,//1ラインサイズ
			(UINT)img->slicePitch//一枚サイズ
		);
		assert(SUCCEEDED(result));
	}
}