#include "Model.h"

	//3D�I�u�W�F�N�g�̏���������
void Model::Initialize(ID3D12Device* device,Input* input, int objectNum){
	assert(device);
	assert(input);

	input_ = input;

	HRESULT result;

#pragma region constMapTransfrom�֘A

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
											//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = (sizeof(ConstBufferDataTransform_) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
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
		IID_PPV_ARGS(&constBuffTransform));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffTransform->Map(0, nullptr,
		(void**)&constMapTransform_); //�}�b�s���O
	assert(SUCCEEDED(result));

#pragma endregion


	//�������火�͐e�q�\���̃T���v��
	//�擪�ȊO�Ȃ�
	//if (objectNum > 0) {
	//	//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
	//	parent_ = &[objectNum - 1];
	//	//�e�I�u�W�F�N�g��9���̑傫��
	//	scale_ = { 0.9f,0.9f,0.9f };
	//	//�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
	//	rotation_ = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
	//	//�e�I�u�W�F�N�g�ɑ΂���Z������-8.0���炷
	//	position_ = { 0.0f,0.0f,-8.0f };
	//}
}


	//�I�u�W�F�N�g�X�V����
void Model::Update(XMMATRIX& matView, XMMATRIX& matProjection){
	
	UpdateObjectControll();


		XMMATRIX matScale, matRot, matTrans;

		//�X�P�[���A��]�A���s�ړ��s��̌v�Z
		matScale = XMMatrixScaling(scale_.x,scale_.y,scale_.z);
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(rotation_.z);
		matRot *= XMMatrixRotationX(rotation_.x);
		matRot *= XMMatrixRotationY(rotation_.y);
		matTrans = XMMatrixTranslation(
			position_.x, position_.y, position_.z);

		//���[���h�s��̍���
		matWorld_ = XMMatrixIdentity();	//�ό`���Z�b�g
		matWorld_ *= matScale;	//���[���h�s��̃X�P�[�����O�𔽉f
		matWorld_ *= matRot;	//���[���h�s��ɉ�]�𔽉f
		matWorld_ *= matTrans;	//���[���h�s��ɕ��s�ړ��𔽉f

		//�e�I�u�W�F�N�g�������
		//if (object_->parent_ != nullptr) {
		//	//�e�I�u�W�F�N�g�̃��[���h�s����|����
		//	object_->matWorld_ *= object_->parent_->matWorld_;
		//}

		//�萔�o�b�t�@�փf�[�^�]��
		constMapTransform_->mat = matWorld_ * matView * matProjection;
}

void Model::Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices){
		//���_�o�b�t�@�̐ݒ�
		commandList->IASetVertexBuffers(0, 1, &vbView);
		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		commandList->IASetIndexBuffer(&ibView);
		//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());

		//�`��R�}���h
		commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

//���W����
void Model::UpdateObjectPosition() {
	if (input_->ifKeyPress(DIK_UP)) { position_.y += 1.0f; }
	else if (input_->ifKeyPress(DIK_DOWN)) { position_.y -= 1.0f; }
	if (input_->ifKeyPress(DIK_RIGHT)) { position_.x += 1.0f; }
	else if (input_->ifKeyPress(DIK_LEFT)) { position_.x -= 1.0f; }
}
//��]����
void Model::UpdateObjectRotation() {
	if (input_->ifKeyPress(DIK_Q)) { rotation_.z += 0.1f; }
	else if (input_->ifKeyPress(DIK_E)) { rotation_.z -= 0.1f; }
}
//�I�u�W�F�N�g����
void Model::UpdateObjectControll() {
	UpdateObjectRotation();
	UpdateObjectPosition();
	if (input_->ifKeyTrigger(DIK_RETURN)) { position_.y -= 6.0f; }

	if (input_->ifKeyRelease(DIK_RETURN)) { position_.y += 20.0f; }
}