#include "Model.h"

void Model::Initialize(Model* model, ID3D12Device* device){
	HRESULT result;

#pragma region constMapTransfrom�֘A

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
											//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&model->constBuffTransform_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = model->constBuffTransform_->Map(0, nullptr,
		(void**)&model->constMapTransform_); //�}�b�s���O
	assert(SUCCEEDED(result));

#pragma endregion
}

void Model::InitializeTexture(TextureData* textureData, const wchar_t* szFile) {
	HRESULT result;

	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&textureData->metadata,
		textureData->scratchImg);

	//�~�b�v�}�b�v����
	result = GenerateMipMaps(
		textureData->scratchImg.GetImages(),
		textureData->scratchImg.GetImageCount(),
		textureData->scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, textureData->mipChine);
	if (SUCCEEDED(result)) {
		textureData->scratchImg = std::move(textureData->mipChine);
		textureData->metadata = textureData->scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾂Ńf�C�q���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	textureData->metadata.format = MakeSRGB(textureData->metadata.format);
};

void Model:: TransferTextureBuffer(TextureData* textureData, ID3D12Device* device) {
	HRESULT result;

	//�q�[�v�ݒ�
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureData->textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData->textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData->textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	textureData->textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData->textureResourceDesc.Format = textureData->metadata.format;
	textureData->textureResourceDesc.Width = textureData->metadata.width; //��
	textureData->textureResourceDesc.Height = (UINT)textureData->metadata.height; //����
	textureData->textureResourceDesc.DepthOrArraySize = (UINT16)textureData->metadata.arraySize;
	textureData->textureResourceDesc.MipLevels = (UINT16)textureData->metadata.mipLevels;
	textureData->textureResourceDesc.SampleDesc.Count = 1;

	////�e�N�X�`���o�b�t�@�̐���
	//ID3D12Resource* texBuff = nullptr;

	result = device->CreateCommittedResource(
		&textureData->textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData->textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData->texBuff));

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < textureData->metadata.mipLevels; i++) {
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = textureData->scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = textureData->texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			(UINT)img->rowPitch,//1���C���T�C�Y
			(UINT)img->slicePitch//�ꖇ�T�C�Y
		);
		assert(SUCCEEDED(result));
	}
}

//3D�I�u�W�F�N�g�̏����������̌Ăяo��
void Model::SetInitialize(Model* model, ID3D12Device* device, int objectNum) {
	//������
	Initialize(model, device);

	//�������火�͐e�q�\���̃T���v��
	//�擪�ȊO�Ȃ�
	if (objectNum > 0) {
		//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
		model->parent_ = &model[objectNum - 1];
		//�e�I�u�W�F�N�g��9���̑傫��
		model->worldTransform_.scale_ = { 0.9f,0.9f,0.9f };
		//�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
		model->worldTransform_.rotation_ = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
		//�e�I�u�W�F�N�g�ɑ΂���Z������-8.0���炷
		model->worldTransform_.position_ = { 0.0f,0.0f,-8.0f };
	}
}

//�I�u�W�F�N�g�X�V����
void Model::Update(Model* model) {
	Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = Mat4Scale(model->worldTransform_.scale_);
	matRota = Mat4Identity();

	matZ = Mat4RotationZ(model->worldTransform_.rotation_.z);
	matX = Mat4RotationX(model->worldTransform_.rotation_.x);
	matY = Mat4RotationY(model->worldTransform_.rotation_.y);

	matRota = matRota * matZ * matX * matY;

	matTrans = Mat4Translation(model->worldTransform_.position_);

	//���[���h�s��̍���
	model->worldTransform_.matWorld_ = Mat4Identity();	//�ό`���Z�b�g

	model->worldTransform_.matWorld_ *= matScale;	//���[���h�s��̃X�P�[�����O�𔽉f
	model->worldTransform_.matWorld_ *= matRota;	//���[���h�s��ɉ�]�𔽉f
	model->worldTransform_.matWorld_ *= matTrans;	//���[���h�s��ɕ��s�ړ��𔽉f

													//�e�I�u�W�F�N�g�������
	if (model->parent_ != nullptr) {
		//�e�I�u�W�F�N�g�̃��[���h�s����|����
		model->worldTransform_.matWorld_ *= model->parent_->worldTransform_.matWorld_;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	model->constMapTransform_->mat = model->worldTransform_.matWorld_ *
		viewProjection_->GetViewProjection().matView_ *
		viewProjection_->GetViewProjection().matPro_ ;
}

void Model:: Draw(Model* model, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices) {
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	commandList->IASetIndexBuffer(&ibView);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, model->constBuffTransform_->GetGPUVirtualAddress());

	//�`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

////���W����
//void UpdateObjectPosition(Object3d* object, Input* input) {
//	if (input->ifKeyPress(DIK_UP)) { object->position_.y += 1.0f; }
//	else if (input->ifKeyPress(DIK_DOWN)) { object->position_.y -= 1.0f; }
//	if (input->ifKeyPress(DIK_RIGHT)) { object->position_.x += 1.0f; }
//	else if (input->ifKeyPress(DIK_LEFT)) { object->position_.x -= 1.0f; }
//
//}
////��]����
//void UpdateObjectRotation(Object3d* object, Input* input) {
//	if (input->ifKeyPress(DIK_Q)) { object->rotation_.z += 0.1f; }
//	else if (input->ifKeyPress(DIK_E)) { object->rotation_.z -= 0.1f; }
//}
////�I�u�W�F�N�g����
//void UpdateObjectControll(Object3d* object, Input* input) {
//	UpdateObjectRotation(object, input);
//	UpdateObjectPosition(object, input);
//	if (input->ifKeyTrigger(DIK_RETURN)) { object->position_.y -= 6.0f; }
//	if (input->ifKeyRelease(DIK_RETURN)) { object->position_.y += 20.0f; }
//}
