#include "Model.h"

void Model::Initialize(Model* model, ID3D12Device* device){
	HRESULT result;

#pragma region constMapTransfrom関連

	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
											//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256バイトアラインメント
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model->constBuffTransform_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = model->constBuffTransform_->Map(0, nullptr,
		(void**)&model->constMapTransform_); //マッピング
	assert(SUCCEEDED(result));

#pragma endregion
}

void Model::InitializeTexture(TextureData* textureData, const wchar_t* szFile) {
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
};

void Model:: TransferTextureBuffer(TextureData* textureData, ID3D12Device* device) {
	HRESULT result;

	//ヒープ設定
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureData->textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData->textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData->textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	textureData->textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData->textureResourceDesc.Format = textureData->metadata.format;
	textureData->textureResourceDesc.Width = textureData->metadata.width; //幅
	textureData->textureResourceDesc.Height = (UINT)textureData->metadata.height; //高さ
	textureData->textureResourceDesc.DepthOrArraySize = (UINT16)textureData->metadata.arraySize;
	textureData->textureResourceDesc.MipLevels = (UINT16)textureData->metadata.mipLevels;
	textureData->textureResourceDesc.SampleDesc.Count = 1;

	////テクスチャバッファの生成
	//ID3D12Resource* texBuff = nullptr;

	result = device->CreateCommittedResource(
		&textureData->textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData->textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData->texBuff));

	//全ミップマップについて
	for (size_t i = 0; i < textureData->metadata.mipLevels; i++) {
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

//3Dオブジェクトの初期化処理の呼び出し
void Model::SetInitialize(Model* model, ID3D12Device* device, int objectNum) {
	//初期化
	Initialize(model, device);

	//ここから↓は親子構造のサンプル
	//先頭以外なら
	if (objectNum > 0) {
		//1つ前のオブジェクトを親オブジェクトとする
		model->parent_ = &model[objectNum - 1];
		//親オブジェクトの9割の大きさ
		model->worldTransform_.scale_ = { 0.9f,0.9f,0.9f };
		//親オブジェクトに対してZ軸まわりに30度回転
		model->worldTransform_.rotation_ = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
		//親オブジェクトに対してZ方向に-8.0ずらす
		model->worldTransform_.position_ = { 0.0f,0.0f,-8.0f };
	}
}

//オブジェクト更新処理
void Model::Update(Model* model) {
	Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	//スケール、回転、平行移動行列の計算
	matScale = Mat4Scale(model->worldTransform_.scale_);
	matRota = Mat4Identity();

	matZ = Mat4RotationZ(model->worldTransform_.rotation_.z);
	matX = Mat4RotationX(model->worldTransform_.rotation_.x);
	matY = Mat4RotationY(model->worldTransform_.rotation_.y);

	matRota = matRota * matZ * matX * matY;

	matTrans = Mat4Translation(model->worldTransform_.position_);

	//ワールド行列の合成
	model->worldTransform_.matWorld_ = Mat4Identity();	//変形リセット

	model->worldTransform_.matWorld_ *= matScale;	//ワールド行列のスケーリングを反映
	model->worldTransform_.matWorld_ *= matRota;	//ワールド行列に回転を反映
	model->worldTransform_.matWorld_ *= matTrans;	//ワールド行列に平行移動を反映

													//親オブジェクトがあれば
	if (model->parent_ != nullptr) {
		//親オブジェクトのワールド行列を掛ける
		model->worldTransform_.matWorld_ *= model->parent_->worldTransform_.matWorld_;
	}

	//定数バッファへデータ転送
	model->constMapTransform_->mat = model->worldTransform_.matWorld_ *
		viewProjection_->GetViewProjection().matView_ *
		viewProjection_->GetViewProjection().matPro_ ;
}

void Model:: Draw(Model* model, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices) {
	//頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//インデックスバッファの設定
	commandList->IASetIndexBuffer(&ibView);
	//定数バッファビュー(CBV)の設定コマンド
	commandList->SetGraphicsRootConstantBufferView(2, model->constBuffTransform_->GetGPUVirtualAddress());

	//描画コマンド
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

////座標操作
//void UpdateObjectPosition(Object3d* object, Input* input) {
//	if (input->ifKeyPress(DIK_UP)) { object->position_.y += 1.0f; }
//	else if (input->ifKeyPress(DIK_DOWN)) { object->position_.y -= 1.0f; }
//	if (input->ifKeyPress(DIK_RIGHT)) { object->position_.x += 1.0f; }
//	else if (input->ifKeyPress(DIK_LEFT)) { object->position_.x -= 1.0f; }
//
//}
////回転操作
//void UpdateObjectRotation(Object3d* object, Input* input) {
//	if (input->ifKeyPress(DIK_Q)) { object->rotation_.z += 0.1f; }
//	else if (input->ifKeyPress(DIK_E)) { object->rotation_.z -= 0.1f; }
//}
////オブジェクト操作
//void UpdateObjectControll(Object3d* object, Input* input) {
//	UpdateObjectRotation(object, input);
//	UpdateObjectPosition(object, input);
//	if (input->ifKeyTrigger(DIK_RETURN)) { object->position_.y -= 6.0f; }
//	if (input->ifKeyRelease(DIK_RETURN)) { object->position_.y += 20.0f; }
//}
