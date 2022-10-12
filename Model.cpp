#include "Model.h"

	//3Dオブジェクトの初期化処理
void Model::Initialize(ID3D12Device* device,Input* input, int objectNum){
	assert(device);
	assert(input);

	input_ = input;

	HRESULT result;

#pragma region constMapTransfrom関連

	//ヒープ設定
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
											//リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform_) + 0xff) & ~0xff; //256バイトアラインメント
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
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuffTransform->Map(0, nullptr,
		(void**)&constMapTransform_); //マッピング
	assert(SUCCEEDED(result));

#pragma endregion


	//ここから↓は親子構造のサンプル
	//先頭以外なら
	//if (objectNum > 0) {
	//	//1つ前のオブジェクトを親オブジェクトとする
	//	parent_ = &[objectNum - 1];
	//	//親オブジェクトの9割の大きさ
	//	scale_ = { 0.9f,0.9f,0.9f };
	//	//親オブジェクトに対してZ軸まわりに30度回転
	//	rotation_ = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
	//	//親オブジェクトに対してZ方向に-8.0ずらす
	//	position_ = { 0.0f,0.0f,-8.0f };
	//}
}


	//オブジェクト更新処理
void Model::Update(XMMATRIX& matView, XMMATRIX& matProjection){
	
	UpdateObjectControll();


		XMMATRIX matScale, matRot, matTrans;

		//スケール、回転、平行移動行列の計算
		matScale = XMMatrixScaling(scale_.x,scale_.y,scale_.z);
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(rotation_.z);
		matRot *= XMMatrixRotationX(rotation_.x);
		matRot *= XMMatrixRotationY(rotation_.y);
		matTrans = XMMatrixTranslation(
			position_.x, position_.y, position_.z);

		//ワールド行列の合成
		matWorld_ = XMMatrixIdentity();	//変形リセット
		matWorld_ *= matScale;	//ワールド行列のスケーリングを反映
		matWorld_ *= matRot;	//ワールド行列に回転を反映
		matWorld_ *= matTrans;	//ワールド行列に平行移動を反映

		//親オブジェクトがあれば
		//if (object_->parent_ != nullptr) {
		//	//親オブジェクトのワールド行列を掛ける
		//	object_->matWorld_ *= object_->parent_->matWorld_;
		//}

		//定数バッファへデータ転送
		constMapTransform_->mat = matWorld_ * matView * matProjection;
}

void Model::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices){
		//頂点バッファの設定
		commandList->IASetVertexBuffers(0, 1, &vbView);
		//インデックスバッファの設定
		commandList->IASetIndexBuffer(&ibView);
		//定数バッファビュー(CBV)の設定コマンド
		commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

		//描画コマンド
		commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

//座標操作
void Model::UpdateObjectPosition() {
	if (input_->ifKeyPress(DIK_UP)) { position_.y += 1.0f; }
	else if (input_->ifKeyPress(DIK_DOWN)) { position_.y -= 1.0f; }
	if (input_->ifKeyPress(DIK_RIGHT)) { position_.x += 1.0f; }
	else if (input_->ifKeyPress(DIK_LEFT)) { position_.x -= 1.0f; }
}
//回転操作
void Model::UpdateObjectRotation() {
	if (input_->ifKeyPress(DIK_Q)) { rotation_.z += 0.1f; }
	else if (input_->ifKeyPress(DIK_E)) { rotation_.z -= 0.1f; }
}
//オブジェクト操作
void Model::UpdateObjectControll() {
	UpdateObjectRotation();
	UpdateObjectPosition();
	if (input_->ifKeyTrigger(DIK_RETURN)) { position_.y -= 6.0f; }

	if (input_->ifKeyRelease(DIK_RETURN)) { position_.y += 20.0f; }
}