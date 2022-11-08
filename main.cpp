#include "WinApp.h"
#include "Input.h"
#include "DirectXBasis.h"
#include "DrawBasis.h"
#include "Sprite.h"

#include <DirectXMath.h>
#include <DirectXTex.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

#include <string>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <d3dcompiler.h>//�V�F�[�_�p�R���p�C��

#include "Struct.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma comment(lib, "d3dcompiler.lib")//�V�F�[�_�p�R���p�C��

using namespace DirectX;
using namespace Microsoft::WRL;

//�萔�o�b�t�@�p�f�[�^�\����(�}�e���A��)
struct ConstBufferDataMaterial {
	Vector4 color; //�F(RGBA)
};

//����05-02�Œǉ�
#pragma region 3D�ϊ��s��
//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
struct ConstBufferDataTransform {
	Matrix4 mat; //3D�ϊ��s��
};

//�萔�o�b�t�@�p�f�[�^�\����(�r���[�v���W�F�N�V����)
struct ConstBufferDataViewProjection {
	Matrix4 view; //�r���[�s��
	Matrix4 projection; //�v���W�F�N�V�����s��
};

//3D�I�u�W�F�N�g�^
struct Object3d {
	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffTransform_ = {};

	//�萔�o�b�t�@(�r���[�v���W�F�N�V����)
	ConstBufferDataViewProjection viewProjection_ = {};

	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform* constMapTransform_ = {};

	//�A�t�B���ϊ����
	Vector3 scale_ = { 1,1,1 };
	Vector3 rotation_ = { 0,0,0 };
	Vector3 position_ = { 0,0,0 };

	//���[���h�ϊ��s��
	Matrix4 matWorld_ = {};

	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent_ = nullptr;
};

struct TextureData {
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	ScratchImage mipChine{};

	D3D12_HEAP_PROPERTIES textureHeapProp{};
	D3D12_RESOURCE_DESC textureResourceDesc{};

	//�e�N�X�`���o�b�t�@�̐���
	ComPtr<ID3D12Resource> texBuff = nullptr;
};

//3D�I�u�W�F�N�g�̏�����
void InitializeObject3d(Object3d* object, ID3D12Device* device) {
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
		IID_PPV_ARGS(&object->constBuffTransform_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = object->constBuffTransform_->Map(0, nullptr,
		(void**)&object->constMapTransform_); //�}�b�s���O
	assert(SUCCEEDED(result));

#pragma endregion
}

void InitializeTexture(TextureData* textureData, const wchar_t* szFile) {
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

void TransferTextureBuffer(TextureData* textureData, ID3D12Device* device) {
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
void SetIntializeObject3ds(Object3d* object, ID3D12Device* device, int objectNum) {
	//������
	InitializeObject3d(object, device);

	//�������火�͐e�q�\���̃T���v��
	//�擪�ȊO�Ȃ�
	if (objectNum > 0) {
		//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
		object->parent_ = &object[objectNum - 1];
		//�e�I�u�W�F�N�g��9���̑傫��
		object->scale_ = { 0.9f,0.9f,0.9f };
		//�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
		object->rotation_ = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
		//�e�I�u�W�F�N�g�ɑ΂���Z������-8.0���炷
		object->position_ = { 0.0f,0.0f,-8.0f };
	}
}

//�I�u�W�F�N�g�X�V����
void UpdateObject3d(Object3d* object) {
	Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = MatScale(object->scale_);
	matRota = MatIdentity();

	matZ = MatRotationZ(object->rotation_.z);
	matX = MatRotationX(object->rotation_.x);
	matY = MatRotationY(object->rotation_.y);

	matRota = matRota * matZ * matX * matY;

	matTrans = MatTranslation(object->position_);

	//���[���h�s��̍���
	object->matWorld_ = MatIdentity();	//�ό`���Z�b�g

	object->matWorld_ *= matScale;	//���[���h�s��̃X�P�[�����O�𔽉f
	object->matWorld_ *= matRota;	//���[���h�s��ɉ�]�𔽉f
	object->matWorld_ *= matTrans;	//���[���h�s��ɕ��s�ړ��𔽉f

	//�e�I�u�W�F�N�g�������
	if (object->parent_ != nullptr) {
		//�e�I�u�W�F�N�g�̃��[���h�s����|����
		object->matWorld_ *= object->parent_->matWorld_;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	object->constMapTransform_->mat = object->matWorld_ *
		object->viewProjection_.view *
		object->viewProjection_.projection;
}

void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices) {
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	commandList->IASetIndexBuffer(&ibView);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform_->GetGPUVirtualAddress());

	//�`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

//���W����
void UpdateObjectPosition(Object3d* object, Input* input) {
	if (input->ifKeyPress(DIK_UP)) { object->position_.y += 1.0f; }
	else if (input->ifKeyPress(DIK_DOWN)) { object->position_.y -= 1.0f; }
	if (input->ifKeyPress(DIK_RIGHT)) { object->position_.x += 1.0f; }
	else if (input->ifKeyPress(DIK_LEFT)) { object->position_.x -= 1.0f; }

}
//��]����
void UpdateObjectRotation(Object3d* object, Input* input) {
	if (input->ifKeyPress(DIK_Q)) { object->rotation_.z += 0.1f; }
	else if (input->ifKeyPress(DIK_E)) { object->rotation_.z -= 0.1f; }
}
//�I�u�W�F�N�g����
void UpdateObjectControll(Object3d* object, Input* input) {
	UpdateObjectRotation(object, input);
	UpdateObjectPosition(object, input);
	if (input->ifKeyTrigger(DIK_RETURN)) { object->position_.y -= 6.0f; }
	if (input->ifKeyRelease(DIK_RETURN)) { object->position_.y += 20.0f; }
}

#pragma region �{��
//
//int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
//	HRESULT result;
//#pragma region ��Տ�����
//#pragma region WindowsAPI
//	//�|�C���^
//	WinApp* winApp = nullptr;
//	//WinApp������
//	winApp = new WinApp();
//	winApp->Initialize();
//#pragma endregion
//#pragma region DirectX���
//#pragma region ���͋@
//	//�|�C���^
//	Input* input = nullptr;
//	//���͂̏�����
//	input = new Input();
//	input->Initialize(winApp);
//#pragma endregion
//#pragma region DX���
//	//�|�C���^
//	DirectXBasis* dXBas = nullptr;
//	//DirectXBasis������
//	dXBas = new DirectXBasis();
//	dXBas->Initialize(winApp);
//#pragma endregion
//#pragma region Draw���
//
//#pragma endregion
//#pragma endregion
//#pragma endregion
//#pragma region �Q�[���V�[���̏����ݒ�
//
//#pragma region ��������R�����g�A�E�g
//
//#pragma region �`���񏉊��ݒ�
//
//	float angle = 0.0f; //�J�����̉�]�p
//
//	//�g�k�{��
//	Vector3 scale;
//	//��]�p
//	Vector3 rotation;
//	//���W
//	Vector3 position;
//
//	//�g�k�{��
//	scale = { 1.0f,1.0f,1.0f };
//	//��]�p
//	rotation = { 00.0f,00.0f,00.0f };
//	//���W
//	position = { 0.0f,0.0f,0.0f };
//
//	//���_�f�[�^�\����
//	struct Vertex {
//		XMFLOAT3 pos;		//xyz���W
//		XMFLOAT3 normal;	//�@���x�N�g��
//		XMFLOAT2 uv;		//uv���W
//	};
//
//	////���_�f�[�^
//	//Vertex vertices[] =
//	//{
//	//	//x		 y		z		u	  v
//	//	{{-50.0f, -50.0f, 0.0f}, {0.0f, 1.0f}},//����
//	//	{{-50.0f,  50.0f, 0.0f}, {0.0f, 0.0f}},//����
//	//	{{ 50.0f, -50.0f, 0.0f}, {1.0f, 1.0f}},//�E��
//	//	{{ 50.0f,  50.0f, 0.0f}, {1.0f, 0.0f}},//�E��
//	//};
//
//	//���_�f�[�^
//	Vertex vertices[] = {
//		//x		 y		z		�@��	u	  v
//		//�O
//		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
//		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
//		{{ 5.0f, -5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
//		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 0.0f}},//�E��
//
//		//���				 
//		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
//		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
//		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 1.0f}},//����
//		{{-5.0f,  5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����
//
//		//��							
//		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
//		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����
//		{{-5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
//		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
//
//		//�E							
//		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
//		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
//		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
//		{{ 5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����
//
//		//��							
//		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
//		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
//		{{-5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
//		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
//
//		//��							
//		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
//		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
//		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
//		{{ 5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
//	};
//
//	//�C���f�b�N�X�f�[�^
//	unsigned short indices[] = {
//		//�O
//		0,1,2,//���
//		2,1,3,//���
//			  //���
//			  4,5,6,//�O��
//			  6,5,7,//�l��
//					//��
//					8,9,10,//���
//					10,9,11,//���
//							//�E
//							12,13,14,
//							14,13,15,
//							//��
//							16,17,18,//���
//							18,17,19,//���
//									 //��
//									 20,21,22,
//									 22,21,23,
//	};
//
//	bool ifOneTextureNum = true;
//
//	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
//	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
//
//	//���_�o�b�t�@�̐ݒ�
//	//�q�[�v�ݒ�
//	D3D12_HEAP_PROPERTIES heapProp{};
//	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
//										   //���\�[�X�ݒ�
//	D3D12_RESOURCE_DESC resDesc{};
//	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resDesc.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
//	resDesc.Height = 1;
//	resDesc.DepthOrArraySize = 1;
//	resDesc.MipLevels = 1;
//	resDesc.SampleDesc.Count = 1;
//	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//���_�o�b�t�@�̐���
//	ComPtr<ID3D12Resource> vertBuff = nullptr;
//	result = dXBas->GetDevice()->CreateCommittedResource(
//		&heapProp,//�q�[�v�ݒ�
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,//���\�[�X�ݒ�
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff));
//	assert(SUCCEEDED(result));
//
//	//GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
//	Vertex* vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
//	assert(SUCCEEDED(result));
//
//	/* vertices�ɋL�� */
//
//	//�S���_�ɑ΂���
//	for (int i = 0; i < _countof(vertices); i++) {
//		vertMap[i] = vertices[i];//���W���R�s�[
//	}
//
//	//�q���������
//	vertBuff->Unmap(0, nullptr);
//
//	//���_�o�b�t�@�r���[�̍쐬
//	D3D12_VERTEX_BUFFER_VIEW vbView{};
//	//GPU���z�A�h���X
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	//���_�o�b�t�@�̃T�C�Y
//	vbView.SizeInBytes = sizeVB;
//	//���_�P���̃f�[�^�T�C�Y
//	vbView.StrideInBytes = sizeof(vertices[0]);
//
//	ComPtr<ID3DBlob> vsBlob = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
//	ComPtr<ID3DBlob> psBlob = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
//	ComPtr<ID3DBlob> errorBlob = nullptr;//�G���[�I�u�W�F�N�g
//
//										 //���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
//	result = D3DCompileFromFile(
//		L"Resources/shaders/BasicVS.hlsl",//�V�F�[�_�t�@�C����
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
//		"main", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�\���f���w��
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
//		0,
//		&vsBlob, &errorBlob);
//
//	//�G���[�Ȃ�
//	if (FAILED(result)) {
//		//errorBlob����G���[���e��string�^�ɃR�s�[
//		std::string error;
//		error.resize(errorBlob->GetBufferSize());
//
//		std::copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			error.begin());
//		error += "\n";
//		//�G���[���e���o�̓E�B���h�E�ɕ\��
//		OutputDebugStringA(error.c_str());
//		assert(0);
//	}
//
//	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
//	result = D3DCompileFromFile(
//		L"Resources/shaders/BasicPS.hlsl",//�V�F�[�_�t�@�C����
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
//		"main", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
//		0,
//		&psBlob, &errorBlob);
//
//	//�G���[�Ȃ�
//	if (FAILED(result)) {
//		//errorBlob����G���[���e��string�^�ɃR�s�[
//		std::string error;
//		error.resize(errorBlob->GetBufferSize());
//
//		std::copy_n((char*)errorBlob->GetBufferPointer(),
//			errorBlob->GetBufferSize(),
//			error.begin());
//		error += "\n";
//		//�G���[���e���o�̓E�B���h�E�ɕ\��
//		OutputDebugStringA(error.c_str());
//		assert(0);
//	}
//
//	//���_���C�A�E�g
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
//		{
//			//xyz���W
//			"POSITION",									//�Z�}���e�B�b�N��
//			0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł悢)
//			DXGI_FORMAT_R32G32B32_FLOAT,				//�v�f���ƃr�b�g����\���@(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT
//			0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
//			D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l�@(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^��ʁ@(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
//			0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
//		},//(1�s�ŏ������������₷������)
//
//		{
//			//�@���x�N�g��
//			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
//			D3D12_APPEND_ALIGNED_ELEMENT,
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
//		},
//
//		//���W�ȊO�� �F�A�e�N�X�`��UI�Ȃǂ�n���ꍇ�͂���ɑ�����
//		{
//			//UV���W
//			"TEXCOORD",
//			0,
//			DXGI_FORMAT_R32G32_FLOAT,
//			0,
//			D3D12_APPEND_ALIGNED_ELEMENT,
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
//			0,
//		},
//		//{/*...*/},
//	};
//
//	//�O���t�B�b�N�X�p�C�v���C���ݒ�
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
//
//	//�V�F�[�_�[�̐ݒ�
//	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
//	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
//	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
//	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
//
//	//�T���v���}�X�N�̐ݒ�
//	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�
//	//pipelineDesc.SampleMask = UINT_MAX;
//
//	//���X�^���C�U�̐ݒ�
//	//�w�ʃJ�����O���ݒ肷��
//	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
//	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//�w�ʃJ�����O����
//	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
//	pipelineDesc.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����
//
//	//�u�����h�X�e�[�g
//	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
//		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB�S�Ẵ`���l����`��
//
//	//�����_�\�^�[�Q�b�g�̃u�����h�ݒ�
//	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
//	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//
//	//�A���t�@�l���ʐݒ�
//	blenddesc.BlendEnable = false; // �u�����h�L���ɂ���
//	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //�u�����h��L���ɂ���
//	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; //���Z
//	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //�f�X�g�̒l�� 0%�g���@
//
//	//���Z����
//	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
//	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
//	//blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��
//
//	//���Z����
//	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //���Z
//	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
//	//blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��
//
//	//�F���]
//	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
//	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-�f�X�g����|�̒l
//	//blenddesc.DestBlend = D3D12_BLEND_ZERO; //�g��Ȃ�
//
//	//����������
//	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
//	blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l���A���t�@�l
//	blenddesc.DestBlend = D3D12_BLEND_ONE; //1.0f-�\�[�X�̃A���t�@�l
//
//	//���_���C�A�E�g�̐ݒ�
//	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
//	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
//
//	//�}�`�̌`��ݒ�
//	pipelineDesc.PrimitiveTopologyType
//		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//
//#pragma region �[�x�e�X�g�̐ݒ�
//	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
//	pipelineDesc.DepthStencilState.DepthEnable = true;
//	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//	pipelineDesc.DSVFormat = DXGI_FORMAT_R32_FLOAT;
//#pragma endregion
//
//	//���̑��̐ݒ�
//	pipelineDesc.NumRenderTargets = 1;//�`��Ώۂ�1��
//	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`255�w���RGBA
//	pipelineDesc.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1��T���v�����O
//
//	//���[�g�V�O�l�`��
//	ComPtr<ID3D12RootSignature> rootSignature;
//
//	//�f�X�N���v�^�����W�̐ݒ�
//	D3D12_DESCRIPTOR_RANGE descriptorRange{};
//	descriptorRange.NumDescriptors = 1;//��x�̕`��Ɏg���e�N�X�`�����ꖇ�Ȃ̂�1
//	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descriptorRange.BaseShaderRegister = 0;//�e�N�X�`�����W�X�^�ԍ�0��
//	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	//�f�X�N���v�^�e�[�u���̐ݒ�
//	D3D12_DESCRIPTOR_RANGE descRange{};
//	descRange.NumDescriptors = 1;//�萔�͈��
//	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //��ʂ͒萔
//	descRange.BaseShaderRegister = 0; //0�ԃX���b�g����
//	descRange.OffsetInDescriptorsFromTableStart =
//		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//	////���[�g�p�����[�^�̐ݒ�
//	D3D12_ROOT_PARAMETER rootParams[3] = {};
//	//�萔�o�b�t�@0��
//	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
//	rootParams[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
//	rootParams[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
//	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
//
//	//�e�N�X�`�����W�X�^0��
//	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//�萔�o�b�t�@�r���[
//	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;					//�萔�o�b�t�@�ԍ�
//	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�t�H���g�l
//	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
//
//	//�萔�o�b�t�@1��
//	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
//	rootParams[2].Descriptor.ShaderRegister = 1;					//�萔�o�b�t�@�ԍ�
//	rootParams[2].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
//	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
//
//	//�e�N�X�`���T���v���[�̐ݒ�
//	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
//	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
//	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
//	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
//	samplerDesc.MinLOD = 0.0f;
//	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
//	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//
//	//���[�g�V�O�l�`���̐ݒ�
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	rootSignatureDesc.pParameters = rootParams; //���[�g�p�����[�^�̐擪�A�h���X
//	rootSignatureDesc.NumParameters = _countof(rootParams); //���[�g�p�����[�^��
//	rootSignatureDesc.pStaticSamplers = &samplerDesc;
//	rootSignatureDesc.NumStaticSamplers = 1;
//
//	//���[�g�V�O�l�`���̃V���A���C�Y
//	ComPtr<ID3DBlob> rootSigBlob;
//	result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&rootSigBlob,
//		&errorBlob);
//	assert(SUCCEEDED(result));
//
//	result = dXBas->GetDevice()->CreateRootSignature(
//		0,
//		rootSigBlob->GetBufferPointer(),
//		rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(&rootSignature));
//	assert(SUCCEEDED(result));
//
//	//�p�C�v���C���Ƀ��[�g�V�O�l�C�`�����Z�b�g
//	pipelineDesc.pRootSignature = rootSignature.Get();
//
//	//�p�C�v���C���X�e�[�g�̐���
//	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
//	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc,
//		IID_PPV_ARGS(&pipelineState));
//	assert(SUCCEEDED(result));
//
//	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
//
//#pragma endregion
//
//#pragma region constMapMaterial�֘A
//
//	//�q�[�v�ݒ�
//	D3D12_HEAP_PROPERTIES cbheapprop{};
//	cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
//											  //���\�[�X�ݒ�
//	D3D12_RESOURCE_DESC cbresdesc{};
//	cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	cbresdesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
//	cbresdesc.Height = 1;
//	cbresdesc.DepthOrArraySize = 1;
//	cbresdesc.MipLevels = 1;
//	cbresdesc.SampleDesc.Count = 1;
//	cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//�萔�o�b�t�@�̐���
//	result = dXBas->GetDevice()->CreateCommittedResource(
//		&cbheapprop, //�q�[�v�ݒ�
//		D3D12_HEAP_FLAG_NONE,
//		&cbresdesc, //���\�[�X�ݒ�
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&constBuffMaterial));
//	assert(SUCCEEDED(result));
//
//	//�萔�o�b�t�@�̃}�b�s���O
//	ConstBufferDataMaterial* constMapMaterial = nullptr;
//	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial); //�}�b�s���O
//
//	// �l���������ނƎ����I�ɓ]�������
//	constMapMaterial->color = Vector4(0.5f, 0.5f, 0.5f, 1.0f); //RGBA�Ŕ������̐�
//
//	//�}�b�s���O����
//	constBuffMaterial->Unmap(0, nullptr);
//	assert(SUCCEEDED(result));
//
//#pragma endregion
//
//#pragma region constMapTransfrom�֘A
//
//	//�q�[�v�ݒ�
//	D3D12_HEAP_PROPERTIES cbHeapProp{};
//	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
//	//���\�[�X�ݒ�
//	D3D12_RESOURCE_DESC cbResourceDesc{};
//	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
//	cbResourceDesc.Height = 1;
//	cbResourceDesc.DepthOrArraySize = 1;
//	cbResourceDesc.MipLevels = 1;
//	cbResourceDesc.SampleDesc.Count = 1;
//	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//#pragma region �O�����I�u�W�F�N�g�̍\����
//
//	//3D�I�u�W�F�N�g�̐�
//	const size_t kObjectCount = 1;
//	//3D�I�u�W�F�N�g�̔z��
//	Object3d object3ds[kObjectCount];
//
//	Vector3 rndScale;
//	Vector3 rndRota;
//	Vector3 rndPos;
//
//	//�z����̑S�I�u�W�F�N�g�ɑ΂���
//	for (int i = 0; i < _countof(object3ds); i++) {
//		SetIntializeObject3ds(&object3ds[i], dXBas->GetDevice(), i);
//
//		//������
//		InitializeObject3d(&object3ds[i], dXBas->GetDevice());
//
//		//�������火�͐e�q�\���̃T���v��
//		//�擪�ȊO�Ȃ�
//		if (i > 0) {
//			rndScale = {
//				0.7f,
//				0.7f,
//				0.7f,
//			};
//
//			rndRota = {
//				XMConvertToRadians(static_cast<float>(rand() % 90)),
//				XMConvertToRadians(static_cast<float>(rand() % 90)),
//				XMConvertToRadians(static_cast<float>(rand() % 90)),
//			};
//
//			rndPos = {
//				static_cast<float>(rand() % 60 - 30),
//				static_cast<float>(rand() % 60 - 30),
//				static_cast<float>(rand() % 60 - 30),
//			};
//
//			object3ds[i].scale_ = rndScale;
//
//			object3ds[i].rotation_ = rndRota;
//
//			object3ds[i].position_ = rndPos;
//
//			//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
//			//object3ds[i].parent = &object3ds[i - 1];
//			////�e�I�u�W�F�N�g��9���̑傫��
//			//object3ds[i].scale = { 0.9f,0.9f,0.9f };
//			////�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
//			//object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f)};
//			////�e�I�u�W�F�N�g�ɑ΂���Z������-8.0���炷
//			//object3ds[i].position = { 0.0f,0.0f,-8.0f };
//		}
//	}
//
//#pragma endregion
//
//#pragma region �P�ʍs��Ŗ��߂���
//#pragma region ���s���e�s��̌v�Z
//
//	////DirectXMath�ŗp�ӂ���Ă���֐��ɒu������
//	//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
//	//	0.0f, window_width,//���[�A�E�[
//	//	window_height, 0.0f,//���[�A��[
//	//	0.0f, 1.0f);//�O�[�A���[
//#pragma endregion
//
//#pragma region �������e�ϊ��s��̌v�Z
//
//	ConstBufferDataViewProjection viewProjection;
//	XMMATRIX matPro;
//	matPro =
//		XMMatrixPerspectiveFovLH(
//			XMConvertToRadians(45.0f),//�㉺��p45�x
//			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
//			0.1f, 1000.0f
//		);//�O�[�A���[
//
//
//#pragma region �r���[�s��̍쐬
//	XMMATRIX matView;
//	//Vector3 eye(0, 0, -100);	//���_���W
//	//Vector3 target(0, 0, 0);	//�����_���W
//	//Vector3 up(0, 1, 0);		//������x�N�g��
//
//	XMFLOAT3 eye(0, 0, -100);	//���_���W
//	XMFLOAT3 target(0, 0, 0);	//�����_���W
//	XMFLOAT3 up(0, 1, 0);		//������x�N�g��
//
//
//	matView = XMMatrixLookAtLH(
//		XMLoadFloat3(&eye),
//		XMLoadFloat3(&target),
//		XMLoadFloat3(&up));
//
//	for (int i = 0; i < 4; i++) {
//		viewProjection.projection.m[i][0] = XMVectorGetX(matPro.r[i]);
//		viewProjection.projection.m[i][1] = XMVectorGetY(matPro.r[i]);
//		viewProjection.projection.m[i][2] = XMVectorGetZ(matPro.r[i]);
//		viewProjection.projection.m[i][3] = XMVectorGetW(matPro.r[i]);
//
//		viewProjection.view.m[i][0] = XMVectorGetX(matView.r[i]);
//		viewProjection.view.m[i][1] = XMVectorGetY(matView.r[i]);
//		viewProjection.view.m[i][2] = XMVectorGetZ(matView.r[i]);
//		viewProjection.view.m[i][3] = XMVectorGetW(matView.r[i]);
//	}
//	//�z����̑S�I�u�W�F�N�g�ɑ΂���
//	for (int i = 0; i < _countof(object3ds); i++) {
//		object3ds[i].viewProjection_ = viewProjection;
//	}
//#pragma endregion
//
//#pragma endregion
//
//#pragma region ���[���h�ϊ��s��
//
//	//Matrix4 matWorld;
//	//matWorld = MatIdentity();
//
//	//Matrix4 matScale; //�X�P�[�����O�s��
//
//	//Matrix4 matRot; //��]�s��
//	//matRot = MatIdentity();
//
//	//Matrix4 matTrans; //���s�ړ��s��
//	//matTrans = MatTranslation(0, 0, 0);
//
//	//matWorld *= matTrans; //���[���h�s��ɕ��s�ړ��𔽉f
//
//#pragma endregion
//
//#pragma endregion
//
//	// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
//	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
//
//	// ���\�[�X�ݒ�
//	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resDesc.Width = sizeIB; // �C���f�b�N�X��񂪓��镪�̃T�C�Y
//	resDesc.Height = 1;
//	resDesc.DepthOrArraySize = 1;
//	resDesc.MipLevels = 1;
//	resDesc.SampleDesc.Count = 1;
//	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//�C���f�b�N�X�o�b�t�@�̐���
//	ComPtr<ID3D12Resource> indexBuff = nullptr;
//	result = dXBas->GetDevice()->CreateCommittedResource(
//		&heapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&indexBuff));
//
//	//�@�C���f�b�N�X�o�b�t�@���}�b�s���O
//	uint16_t* indexMap = nullptr;
//	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
//	// �S�C���f�b�N�X�ɑ΂���
//	for (int i = 0; i < _countof(indices); i++) {
//		indexMap[i] = indices[i]; //�C���f�b�N�X���R�s�[
//	}
//	//�}�b�s���O����
//	indexBuff->Unmap(0, nullptr);
//
//	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
//	D3D12_INDEX_BUFFER_VIEW ibView{};
//	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
//	ibView.Format = DXGI_FORMAT_R16_UINT;
//	ibView.SizeInBytes = sizeIB;
//
//	const int kTextureCount = 2;
//	TextureData textureDatas[kTextureCount] = { 0 };
//
//	const wchar_t* texImgs[kTextureCount] = {
//		L"Resources/texture.png",
//		L"Resources/reimu.png",
//	};
//
//	for (size_t i = 0; i < _countof(textureDatas); i++) {
//		InitializeTexture(&textureDatas[i], texImgs[i]);
//	}
//
//	for (size_t i = 0; i < _countof(textureDatas); i++) {
//		TransferTextureBuffer(&textureDatas[i], dXBas->GetDevice());
//	}
//
//	//���f�[�^�J��
//	//delete[] imageData;
//
//	//SRV�̍ő��
//	const size_t kMaxSRVCount = 2056;
//
//	//�f�X�N���v�^�q�[�v�̐ݒ�
//	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
//	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
//	srvHeapDesc.NumDescriptors = kMaxSRVCount;
//
//	//�ݒ�����SRV�p�f�X�N���v�^�q�[�v�𐶐�
//	ID3D12DescriptorHeap* srvHeap = nullptr;
//	result = dXBas->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
//	assert(SUCCEEDED(result));
//
//	//SRV�q�[�v�̐擪�n���h�����擾
//	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
//
//	//�V�F�[�_���\�[�X�r���[�ݒ�
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
//	srvDesc.Format = resDesc.Format;//RGBA float
//	srvDesc.Shader4ComponentMapping =
//		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
//	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;
//
//	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
//	dXBas->GetDevice()->CreateShaderResourceView(textureDatas[0].texBuff.Get(), &srvDesc, srvHandle);
//
//#pragma region �e�N�X�`���̍����ւ��ŒǋL
//	//�T�C�Y�ύX
//	UINT incrementSize = dXBas->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	srvHandle.ptr += incrementSize;
//
//	//2���ڗp
//	//�V�F�[�_���\�[�X�r���[�ݒ�
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//�ݒ�\����
//	srvDesc2.Format = textureDatas[1].textureResourceDesc.Format;//RGBA float
//	srvDesc2.Shader4ComponentMapping =
//		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
//	srvDesc2.Texture2D.MipLevels = textureDatas[1].textureResourceDesc.MipLevels;
//
//	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
//	dXBas->GetDevice()->CreateShaderResourceView(textureDatas[1].texBuff.Get(), &srvDesc2, srvHandle);
//
//#pragma endregion
//
//	//CBV,SRV,UAV��1���̃T�C�Y���擾
//	UINT descriptorSize = dXBas->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	//SRV�q�[�v�̐擪�n���h�����擾
//	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
//	//�n���h������i�߂�(SRV�̈ʒu)
//	srvHandle.ptr += descriptorSize * 1;
//
//	//CBV(�R���X�^���g�o�b�t�@�r���[)�̐ݒ�
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
//	//cbvDesc�̒l�ݒ�(�ȗ�)
//	dXBas->GetDevice()->CreateConstantBufferView(&cbvDesc, srvHandle);
//
//#pragma endregion
//
//#pragma endregion
//
//#pragma endregion
//
//#pragma region �Q�[�����[�v
//
//	while (true) {
//#pragma region ��ՍX�V
//		//windows�̃��b�Z�[�W����
//		if (winApp->ProcessMessage()) {
//			//���[�v�𔲂���
//			break;
//		}
//
//		//���͍X�V
//		input->Update();
//#pragma endregion
//
//#pragma region �Q�[���V�[���̍X�V
//
//#pragma region �^�[�Q�b�g�̎�������J����
//		if (input->ifKeyPress(DIK_D) || input->ifKeyPress(DIK_A)) {
//			if (input->ifKeyPress(DIK_D)) { angle += XMConvertToRadians(1.0f); }
//			else if (input->ifKeyPress(DIK_A)) { angle -= XMConvertToRadians(1.0f); }
//
//			//angle���W�A������Y������ɉ�]�A���a��-100
//			eye.x = -100 * sinf(angle);
//			eye.z = -100 * cosf(angle);
//			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
//				XMLoadFloat3(&target), XMLoadFloat3(&up));
//
//			for (int i = 0; i < 4; i++) {
//				viewProjection.view.m[i][0] = XMVectorGetX(matView.r[i]);
//				viewProjection.view.m[i][1] = XMVectorGetY(matView.r[i]);
//				viewProjection.view.m[i][2] = XMVectorGetZ(matView.r[i]);
//				viewProjection.view.m[i][3] = XMVectorGetW(matView.r[i]);
//			}
//
//			object3ds[0].viewProjection_ = viewProjection;
//
//			object3ds[0].constMapTransform_->mat =
//				object3ds[0].viewProjection_.view * object3ds[0].viewProjection_.projection;
//		}
//#pragma endregion
//
//#pragma region �A���ړ�
//
//		UpdateObjectControll(&object3ds[0], input);
//
//		for (size_t i = 0; i < _countof(object3ds); i++) {
//			UpdateObject3d(&object3ds[i]);
//		}
//
//#pragma endregion
//
//#pragma endregion
//
//		//�`��̏���
//		dXBas->PrepareDraw();
//
//#pragma region �Q�[���V�[���̕`��
//
//#pragma region �@�����v�Z
//		for (int i = 0; i < _countof(indices) / 3; i++) {
//			//�O�p�`�P���ƂɌv�Z���Ă���
//			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
//			unsigned short index0 = indices[i * 3 + 0];
//			unsigned short index1 = indices[i * 3 + 1];
//			unsigned short index2 = indices[i * 3 + 2];
//
//			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
//			XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
//			XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
//			XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);
//
//			//p0->p1�x�N�g���A p0->p2�x�N�g�����v�Z (�x�N�g���̌��Z)
//			XMVECTOR v1 = XMVectorSubtract(p1, p0);
//			XMVECTOR v2 = XMVectorSubtract(p2, p0);
//
//			//�O�ς͗������琂���ȃx�N�g��
//			XMVECTOR normal = XMVector3Cross(v1, v2);
//
//			//���K�� (������1�ɂ���)
//			normal = XMVector3Normalize(normal);
//
//			//���߂��@���𒸓_�f�[�^�ɑ��
//			XMStoreFloat3(&vertices[index0].normal, normal);
//			XMStoreFloat3(&vertices[index1].normal, normal);
//			XMStoreFloat3(&vertices[index2].normal, normal);
//		}
//#pragma endregion
//
//#pragma region �]��
//		//�S���_�ɑ΂���
//		for (int i = 0; i < _countof(vertices); i++) {
//			vertMap[i] = vertices[i];//���W���R�s�[
//		}
//#pragma endregion
//
//#pragma region �ݒ�R�}���h
//
//		//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
//		dXBas->GetCommandList()->SetPipelineState(pipelineState.Get());
//		dXBas->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
//
//		//�v���~�e�B�u�`��̐ݒ�R�}���h
//		dXBas->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g
//
//		//���_�o�b�t�@�r���[�̐ݒ�R�}���h
//		dXBas->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
//
//		//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
//		dXBas->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
//
//		//SRV�q�[�v�̐ݒ�R�}���h
//		dXBas->GetCommandList()->SetDescriptorHeaps(1, &srvHeap);
//
//		//SRV�q�[�v�̐擪�n���h�����擾(SRV���w���Ă���͂�)
//		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
//		//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
//		dXBas->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
//
//		if (ifOneTextureNum == false) {
//			//2���ڂ��w�������悤�ɂ���SRV�̃n���h�������[�g�p�����[�^�ɐݒ�
//			srvGpuHandle.ptr += incrementSize;
//			dXBas->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
//		}
//
//		//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
//		dXBas->GetCommandList()->IASetIndexBuffer(&ibView);
//
//		//�S�I�u�W�F�N�g�ɂ��ď���
//		for (int i = 0; i < _countof(object3ds); i++) {
//			DrawObject3d(&object3ds[i], dXBas->GetCommandList(), vbView, ibView, _countof(indices));
//		}
//
//#pragma endregion
//
//#pragma endregion
//
//		//�`��㏈��
//		dXBas->PostDraw();
//	}
//
//#pragma endregion
//
//#pragma region �Q�[���V�[���̔j��
//
//#pragma endregion
//
//#pragma region ��ՏI������
//
//	//WindowsAPI�I������
//	winApp->Finalize();
//
//	//Sprite�̉��
//
//
//	//DrawBasis��Ղ̉��
//
//	//���͂̉��
//	delete input;
//	input = nullptr;
//
//	//DirectX��Ղ̉��
//	delete dXBas;
//	dXBas = nullptr;
//
//	//WinApp�̉��
//	delete winApp;
//	winApp = nullptr;
//
//#pragma endregion
//
//	return 0;
//}

#pragma endregion

#pragma region �f�o�b�O�p

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//HRESULT result;
#pragma region ��Տ�����
#pragma region WindowsAPI
	//�|�C���^
	WinApp* winApp = nullptr;
	//WinApp������
	winApp = new WinApp();
	winApp->Initialize();
#pragma endregion
#pragma region DirectX���
#pragma region ���͋@
	//�|�C���^
	Input* input = nullptr;
	//���͂̏�����
	input = new Input();
	input->Initialize(winApp);
#pragma endregion
#pragma region DX���
	//�|�C���^
	DirectXBasis* dXBas = nullptr;
	//DirectXBasis������
	dXBas = new DirectXBasis();
	dXBas->Initialize(winApp);
#pragma endregion
#pragma region Draw���
	//�|�C���^
	DrawBasis* drawBas = nullptr;
	//DrawBasis������
	drawBas = new DrawBasis();
	drawBas->Initialize(dXBas);
#pragma endregion
#pragma endregion
#pragma endregion
#pragma region �Q�[���V�[���̏����ݒ�
	//�|�C���^
	Sprite* sprite = new Sprite();
	//Sprite������
	sprite->Initialize(drawBas);

#pragma endregion

#pragma region �Q�[�����[�v

	while (true) {
#pragma region ��ՍX�V
		//windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage()) {
			//���[�v�𔲂���
			break;
		}

		//���͍X�V
		input->Update();
#pragma endregion

		//�`��̏���
		dXBas->PrepareDraw();

		//�`��{��
		sprite->Draw();

		//�`��㏈��
		dXBas->PostDraw();
	}

#pragma endregion

#pragma region �Q�[���V�[���̔j��

#pragma endregion

#pragma region ��ՏI������

	//WindowsAPI�I������
	winApp->Finalize();

	//Sprite�̉��
	delete sprite;
	sprite = nullptr;

	//DrawBasis��Ղ̉��
	delete drawBas;
	drawBas = nullptr;

	//���͂̉��
	delete input;
	input = nullptr;

	//DirectX��Ղ̉��
	delete dXBas;
	dXBas = nullptr;

	//WinApp�̉��
	delete winApp;
	winApp = nullptr;

#pragma endregion

	return 0;
}

#pragma endregion

#pragma region �m�F�p
//
//int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
//	//HRESULT result;
//#pragma region ��Տ�����
//#pragma region WindowsAPI
//	//�|�C���^
//	WinApp* winApp = nullptr;
//	//WinApp������
//	winApp = new WinApp();
//	winApp->Initialize();
//#pragma endregion
//#pragma region DirectX���
//#pragma region ���͋@
//	//�|�C���^
//	Input* input = nullptr;
//	//���͂̏�����
//	input = new Input();
//	input->Initialize(winApp);
//#pragma endregion
//#pragma region DX���
//	//�|�C���^
//	DirectXBasis* dXBas = nullptr;
//	//DirectXBasis������
//	dXBas = new DirectXBasis();
//	dXBas->Initialize(winApp);
//#pragma endregion
//#pragma region Draw���
//	//�|�C���^
//	//DrawBasis* drawBas = nullptr;
//	//DrawBasis������
//	//drawBas = new DrawBasis();
//	//drawBas->Initialize(dXBas);
//
//	HRESULT result;
//	static const int ElementDescNum = 1;//inputLayout_�̃G�������g��
//	static const int VerticesNum = 3;//vertices�̒��_��
//
//	D3D12_VERTEX_BUFFER_VIEW vbView_{};//���_�o�b�t�@�r���[
//
//	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
//	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
//	ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g
//
//	D3D12_INPUT_ELEMENT_DESC inputLayout_[ElementDescNum]{};//���_���C�A�E�g
//
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc_{};//�O���t�B�b�N�X�p�C�v���C���f�X�N
//
//	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;//���[�g�V�O�l�`��
//
//	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;//�p�C�v���C���X�e�[�g
//
//	typedef enum VerticesParts {
//		LeftBottom,
//		LeftTop,
//		RightBottom,
//	}VerticesParts;
//
//	float left = -5.0f;
//	float right = +5.0f;
//	float top = +5.0f;
//	float bottom = -5.0f;
//
//	//���_�f�[�^
//	Vector3 vertices[VerticesNum];
//
//	vertices[LeftBottom] = Vector3( left,bottom,0 );
//	vertices[LeftTop] = Vector3( left,top,0 );
//	vertices[RightBottom] = Vector3( right,bottom,0 );
//	//AssembleVetices();
//
//	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
//	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));
//
//	//���_�o�b�t�@�̐ݒ�
//	//�q�[�v�ݒ�
//	D3D12_HEAP_PROPERTIES heapProp{};
//	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
//										   //���\�[�X�ݒ�
//	D3D12_RESOURCE_DESC resDesc{};
//	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resDesc.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
//	resDesc.Height = 1;
//	resDesc.DepthOrArraySize = 1;
//	resDesc.MipLevels = 1;
//	resDesc.SampleDesc.Count = 1;
//	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	//���_�o�b�t�@�̐���
//	ComPtr<ID3D12Resource> vertBuff = nullptr;
//	result = dXBas->GetDevice()->CreateCommittedResource(
//		&heapProp,//�q�[�v�ݒ�
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,//���\�[�X�ݒ�
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff));
//	assert(SUCCEEDED(result));
//
//	//GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
//	Vector3* vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
//	assert(SUCCEEDED(result));
//
//	//�S���_�ɑ΂���
//	for (int i = 0; i < _countof(vertices); i++) {
//		vertMap[i] = vertices[i];//���W���R�s�[
//	}
//
//	//�q���������
//	vertBuff->Unmap(0, nullptr);
//
//	//���_�o�b�t�@�r���[�̍쐬
//	/*D3D12_VERTEX_BUFFER_VIEW vbView{};*/
//	//GPU���z�A�h���X
//	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
//	//���_�o�b�t�@�̃T�C�Y
//	vbView_.SizeInBytes = sizeVB;
//	//���_�P���̃f�[�^�T�C�Y
//	vbView_.StrideInBytes = sizeof(Vector3);
//
//	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
//	result = D3DCompileFromFile(
//		L"Resources/shaders/SpriteVS.hlsl",//�V�F�[�_�t�@�C����
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
//		"main", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�\���f���w��
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
//		0,
//		&vsBlob_, &errorBlob_);
//
//	//�G���[�Ȃ�
//	if (FAILED(result)) {
//		//errorBlob����G���[���e��string�^�ɃR�s�[
//		std::string error;
//		error.resize(errorBlob_->GetBufferSize());
//
//		std::copy_n((char*)errorBlob_->GetBufferPointer(),
//			errorBlob_->GetBufferSize(),
//			error.begin());
//		error += "\n";
//		//�G���[���e���o�̓E�B���h�E�ɕ\��
//		OutputDebugStringA(error.c_str());
//		assert(0);
//	}
//
//	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
//	result = D3DCompileFromFile(
//		L"Resources/shaders/SpritePS.hlsl",//�V�F�[�_�t�@�C����
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
//		"main", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
//		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
//		0,
//		&psBlob_, &errorBlob_);
//
//	//�G���[�Ȃ�
//	if (FAILED(result)) {
//		//errorBlob����G���[���e��string�^�ɃR�s�[
//		std::string error;
//		error.resize(errorBlob_->GetBufferSize());
//
//		std::copy_n((char*)errorBlob_->GetBufferPointer(),
//			errorBlob_->GetBufferSize(),
//			error.begin());
//		error += "\n";
//		//�G���[���e���o�̓E�B���h�E�ɕ\��
//		OutputDebugStringA(error.c_str());
//		assert(0);
//	}
//
//	typedef enum ElementName {
//		Position,
//	}ElementName;
//
//	//���_���C�A�E�g
//	inputLayout_[ElementName::Position] = {
//		//xyz���W
//		"POSITION",									//�Z�}���e�B�b�N��
//		0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł悢)
//		DXGI_FORMAT_R32G32B32_FLOAT,				//�v�f���ƃr�b�g����\���@(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT
//		0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
//		D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l�@(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^��ʁ@(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
//		0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
//	};
//
//	//�V�F�[�_�[�̐ݒ�
//	pipelineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
//	pipelineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
//	pipelineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
//	pipelineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();
//
//	//�T���v���}�X�N�̐ݒ�
//	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�
//
//														 //���X�^���C�U�̐ݒ�
//	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
//	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
//	pipelineDesc_.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����
//
//														 //�u�����h�X�e�[�g
//	pipelineDesc_.BlendState.RenderTarget[0].RenderTargetWriteMask
//		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB�S�Ẵ`���l����`��
//
//									   //���_���C�A�E�g�̐ݒ�
//	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout_;
//	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout_);//_countof(inputLayout_);
//
//																   //�}�`�̌`��ݒ�
//	pipelineDesc_.PrimitiveTopologyType
//		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//
//	//���̑��̐ݒ�
//	pipelineDesc_.NumRenderTargets = 1;//�`��Ώۂ�1��
//	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`255�w���RGBA
//	pipelineDesc_.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1��T���v�����O
//
//									   //���[�g�V�O�l�`��
//									   //ComPtr<ID3D12RootSignature> rootSignature;
//
//									   //���[�g�V�O�l�`���̐ݒ�
//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
//	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	//���[�g�V�O�l�`���̃V���A���C�Y
//	ComPtr<ID3DBlob> rootSigBlob;
//	result = D3D12SerializeRootSignature(
//		&rootSignatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&rootSigBlob,
//		&errorBlob_);
//	assert(SUCCEEDED(result));
//
//	//���[�g�V�O�l�`���̐���
//	result = dXBas->GetDevice()->CreateRootSignature(
//		0,
//		rootSigBlob->GetBufferPointer(),
//		rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(&rootSignature_));
//	assert(SUCCEEDED(result));
//
//	//�p�C�v���C���Ƀ��[�g�V�O�l�C�`�����Z�b�g
//	pipelineDesc_.pRootSignature = rootSignature_.Get();
//
//	//�p�C�v���C���X�e�[�g�̐���
//	/*ComPtr<ID3D12PipelineState> pipelineState = nullptr;*/
//	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
//		IID_PPV_ARGS(&pipelineState_));
//	assert(SUCCEEDED(result));
//#pragma endregion
//#pragma endregion
//#pragma endregion
//#pragma region �Q�[���V�[���̏����ݒ�
//	//�|�C���^
//	//Sprite* sprite = new Sprite();
//	//Sprite������
//	//sprite->Initialize();
//
//#pragma endregion
//
//#pragma region �Q�[�����[�v
//
//	while (true) {
//#pragma region ��ՍX�V
//		//windows�̃��b�Z�[�W����
//		if (winApp->ProcessMessage()) {
//			//���[�v�𔲂���
//			break;
//		}
//
//		//���͍X�V
//		input->Update();
//#pragma endregion
//
//		//�`��̏���
//		dXBas->PrepareDraw();
//
//		//�`��{��
//		//drawBas->Draw(dXBas);
//		//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
//		dXBas->GetCommandList()->SetPipelineState(pipelineState_.Get());
//		dXBas->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
//
//		//�v���~�e�B�u�`��̐ݒ�R�}���h
//		dXBas->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g
//
//																							 //���_�o�b�t�@�r���[�̐ݒ�R�}���h
//		dXBas->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);
//
//		//�`��R�}���h
//		dXBas->GetCommandList()->DrawInstanced(
//			//_countof(vertices)
//			VerticesNum, 1, 0, 0);
//
//
//		//�`��㏈��
//		dXBas->PostDraw();
//	}
//
//#pragma endregion
//
//#pragma region �Q�[���V�[���̔j��
//
//#pragma endregion
//
//#pragma region ��ՏI������
//
//	//WindowsAPI�I������
//	winApp->Finalize();
//
//	//Sprite�̉��
//	//delete sprite;
//	//sprite = nullptr;
//
//	//DrawBasis��Ղ̉��
//	//delete drawBas;
//	//drawBas = nullptr;
//
//	//���͂̉��
//	delete input;
//	input = nullptr;
//
//	//DirectX��Ղ̉��
//	delete dXBas;
//	dXBas = nullptr;
//
//	//WinApp�̉��
//	delete winApp;
//	winApp = nullptr;
//
//#pragma endregion
//
//	return 0;
//}

#pragma endregion