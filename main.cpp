#include "WinApp.h"
#include "Input.h"
#include "DirectXBasis.h"
#include "Drawer.h"

#include <DirectXMath.h>
#include <DirectXTex.h>

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

//����05-02�Œǉ�
#pragma region 3D�ϊ��s��
//�萔�o�b�t�@�p�f�[�^�\����(3D�ϊ��s��)
struct ConstBufferDataTransform {
	XMMATRIX mat; //3D�ϊ��s��
};

//3D�I�u�W�F�N�g�^
struct Object3d
{
	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffTransform = {};

	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform* constMapTransform = {};

	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };

	//���[���h�ϊ��s��
	XMMATRIX matWorld = {};

	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3d* parent = nullptr;
};

//3D�I�u�W�F�N�g�̏�����
void InitializeObject3d(Object3d* object, ID3D12Device* device)
{
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
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = object->constBuffTransform->Map(0, nullptr,
		(void**)&object->constMapTransform); //�}�b�s���O
	assert(SUCCEEDED(result));

#pragma endregion
}


//3D�I�u�W�F�N�g�̏����������̌Ăяo��
void SetIntializeObject3ds(Object3d* object, ID3D12Device* device, int objectNum)
{
	//������
	InitializeObject3d(object, device);

	//�������火�͐e�q�\���̃T���v��
	//�擪�ȊO�Ȃ�
	if (objectNum > 0) {
		//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
		object->parent = &object[objectNum - 1];
		//�e�I�u�W�F�N�g��9���̑傫��
		object->scale = { 0.9f,0.9f,0.9f };
		//�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
		object->rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };
		//�e�I�u�W�F�N�g�ɑ΂���Z������-8.0���炷
		object->position = { 0.0f,0.0f,-8.0f };
	}
}

//�I�u�W�F�N�g�X�V����
void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection)
{
	XMMATRIX matScale, matRot, matTrans;

	//�X�P�[���A��]�A���s�ړ��s��̌v�Z
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);
	matTrans = XMMatrixTranslation(
		object->position.x, object->position.y, object->position.z);

	//���[���h�s��̍���
	object->matWorld = XMMatrixIdentity();	//�ό`���Z�b�g
	object->matWorld *= matScale;	//���[���h�s��̃X�P�[�����O�𔽉f
	object->matWorld *= matRot;	//���[���h�s��ɉ�]�𔽉f
	object->matWorld *= matTrans;	//���[���h�s��ɕ��s�ړ��𔽉f

	//�e�I�u�W�F�N�g�������
	if (object->parent != nullptr) {
		//�e�I�u�W�F�N�g�̃��[���h�s����|����
		object->matWorld *= object->parent->matWorld;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	object->constMapTransform->mat = object->matWorld * matView * matProjection;
}

void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vbView,
	D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices) {
	//���_�o�b�t�@�̐ݒ�
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	commandList->IASetIndexBuffer(&ibView);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	commandList->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());

	//�`��R�}���h
	commandList->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}

//���W����
void UpdateObjectPosition(Object3d* object, Input* input) {
	if (input->ifKeyPress(DIK_UP)) { object->position.y += 1.0f; }
	else if (input->ifKeyPress(DIK_DOWN)) { object->position.y -= 1.0f; }
	if (input->ifKeyPress(DIK_RIGHT)) { object->position.x += 1.0f; }
	else if (input->ifKeyPress(DIK_LEFT)) { object->position.x -= 1.0f; }
}
//��]����
void UpdateObjectRotation(Object3d* object, Input* input) {
	if (input->ifKeyPress(DIK_Q)) { object->rotation.z += 0.1f; }
	else if (input->ifKeyPress(DIK_E)) { object->rotation.z -= 0.1f; }
}
//�I�u�W�F�N�g����
void UpdateObjectControll(Object3d* object, Input* input) {
	UpdateObjectRotation(object, input);
	UpdateObjectPosition(object, input);
	if (input->ifKeyTrigger(DIK_RETURN)) { object->position.y -= 6.0f; }

	if (input->ifKeyRelease(DIK_RETURN)) { object->position.y += 20.0f; }
}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//------WindowsAPI���������� ��������------
	HRESULT result;
	const float PI = 3.1415926535f;

	//�|�C���^
	WinApp* winApp = nullptr;
	//WinApp������
	winApp = new WinApp();
	winApp->Initialize();

	//------WindowsAPI���������� �����܂�------

	//------DirectX���������� ��������------
	//�|�C���^
	Input* input = nullptr;
	//���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	//�|�C���^
	DirectXBasis* dXBas = nullptr;
	//DirectXBasis������
	dXBas = new DirectXBasis();
	dXBas->Initialize(winApp);
	//------DirectX���������� �����܂�------

	//------�`�揉�������� ��������------
#pragma region
	float angle = 0.0f; //�J�����̉�]�p

	//�g�k�{��
	XMFLOAT3 scale;
	//��]�p
	XMFLOAT3 rotation;
	//���W
	XMFLOAT3 position;
	//���W
	//XMFLOAT3 position1;

	//�g�k�{��
	scale = { 1.0f,1.0f,1.0f };
	//��]�p
	rotation = { 00.0f,00.0f,00.0f };
	//���W
	position = { 0.0f,0.0f,0.0f };
	//position1 = { -20.0f,0.0f,0.0f };

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT3 normal;	//�@���x�N�g��
		XMFLOAT2 uv;		//uv���W
	};

	////���_�f�[�^
	//Vertex vertices[] =
	//{
	//	//x		 y		z		u	  v
	//	{{-50.0f, -50.0f, 0.0f}, {0.0f, 1.0f}},//����
	//	{{-50.0f,  50.0f, 0.0f}, {0.0f, 0.0f}},//����
	//	{{ 50.0f, -50.0f, 0.0f}, {1.0f, 1.0f}},//�E��
	//	{{ 50.0f,  50.0f, 0.0f}, {1.0f, 0.0f}},//�E��
	//};

	////���_�f�[�^
	//Vertex vertices[] =
	//{
	//	//x		 y		z		�@��	u	  v
	//	//�O
	//	{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
	//	{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
	//	{{ 5.0f, -5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
	//	{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 0.0f}},//�E��

	//	//���				 
	//	{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
	//	{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
	//	{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 1.0f}},//����
	//	{{-5.0f,  5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����

	//	//��							
	//	{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
	//	{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����
	//	{{-5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
	//	{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��

	//	//�E							
	//	{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
	//	{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
	//	{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
	//	{{ 5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����

	//	//��							
	//	{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
	//	{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
	//	{{-5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
	//	{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��

	//	//��							
	//	{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
	//	{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
	//	{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
	//	{{ 5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
	//};

	////�C���f�b�N�X�f�[�^
	//unsigned short indices[] =
	//{
	//	//�O
	//	0,1,2,//���
	//	2,1,3,//���
	//	//���
	//	4,5,6,//�O��
	//	6,5,7,//�l��
	//	//��
	//	8,9,10,//���
	//	10,9,11,//���
	//	//�E
	//	12,13,14,
	//	14,13,15,
	//	//��
	//	16,17,18,//���
	//	18,17,19,//���
	//	//��
	//	20,21,22,
	//	22,21,23,
	//};

	//bool ifOneTextureNum = true;

	////���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	//UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	////���_�o�b�t�@�̐ݒ�
	////�q�[�v�ݒ�
	//D3D12_HEAP_PROPERTIES heapProp{};
	//heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
	//									   //���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC resDesc{};
	//resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//resDesc.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	//resDesc.Height = 1;
	//resDesc.DepthOrArraySize = 1;
	//resDesc.MipLevels = 1;
	//resDesc.SampleDesc.Count = 1;
	//resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	////���_�o�b�t�@�̐���
	//ComPtr<ID3D12Resource> vertBuff = nullptr;
	//result = dXBas->GetDevice()->CreateCommittedResource(
	//	&heapProp,//�q�[�v�ݒ�
	//	D3D12_HEAP_FLAG_NONE,
	//	&resDesc,//���\�[�X�ݒ�
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&vertBuff));
	//assert(SUCCEEDED(result));

	////GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
	//Vertex* vertMap = nullptr;
	//result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	//assert(SUCCEEDED(result));

	///* vertices�ɋL�� */

	////�S���_�ɑ΂���
	//for (int i = 0; i < _countof(vertices); i++)
	//{
	//	vertMap[i] = vertices[i];//���W���R�s�[
	//}

	////�q���������
	//vertBuff->Unmap(0, nullptr);

	////���_�o�b�t�@�r���[�̍쐬
	//D3D12_VERTEX_BUFFER_VIEW vbView{};
	////GPU���z�A�h���X
	//vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	////���_�o�b�t�@�̃T�C�Y
	//vbView.SizeInBytes = sizeVB;
	////���_�P���̃f�[�^�T�C�Y
	//vbView.StrideInBytes = sizeof(vertices[0]);


	//Drawer�ɂ͍��A���̕ӂ�̏����������z�����Ă܂�
	//�|�C���^
	Drawer* drawer = nullptr;
	//Drawer������
	drawer = new Drawer();
	drawer->Initialize(dXBas,L"BasicVS.hlsl",L"BasicPS.hlsl");

#pragma region constMapTransfrom�֘A

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

#pragma region �O�����I�u�W�F�N�g�̍\����

	//3D�I�u�W�F�N�g�̐�
	const size_t kObjectCount = 1;
	//3D�I�u�W�F�N�g�̔z��
	Object3d object3ds[kObjectCount];

	XMFLOAT3 rndScale;
	XMFLOAT3 rndRota;
	XMFLOAT3 rndPos;

	//�z����̑S�I�u�W�F�N�g�ɑ΂���
	for (int i = 0; i < _countof(object3ds); i++)
	{
		SetIntializeObject3ds(&object3ds[i],dXBas->GetDevice(), i);

		//������
		InitializeObject3d(&object3ds[i], dXBas->GetDevice());

		//�������火�͐e�q�\���̃T���v��
		//�擪�ȊO�Ȃ�
		if (i > 0) {
			rndScale = {
				0.7f,
				0.7f,
				0.7f,
			};

			rndRota = {
				XMConvertToRadians(static_cast<float>(rand() % 90)),
				XMConvertToRadians(static_cast<float>(rand() % 90)),
				XMConvertToRadians(static_cast<float>(rand() % 90)),
			};

			rndPos = {
				static_cast<float>(rand() % 60 - 30),
				static_cast<float>(rand() % 60 - 30),
				static_cast<float>(rand() % 60 - 30),
			};

			object3ds[i].scale = rndScale;

			object3ds[i].rotation = rndRota;

			object3ds[i].position = rndPos;

			//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
			//object3ds[i].parent = &object3ds[i - 1];
			////�e�I�u�W�F�N�g��9���̑傫��
			//object3ds[i].scale = { 0.9f,0.9f,0.9f };
			////�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
			//object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f)};
			////�e�I�u�W�F�N�g�ɑ΂���Z������-8.0���炷
			//object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}

	}

#pragma endregion

#pragma region �P�ʍs��Ŗ��߂���
#pragma region ���s���e�s��̌v�Z

	////DirectXMath�ŗp�ӂ���Ă���֐��ɒu������
	//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
	//	0.0f, window_width,//���[�A�E�[
	//	window_height, 0.0f,//���[�A��[
	//	0.0f, 1.0f);//�O�[�A���[
#pragma endregion

#pragma region �������e�ϊ��s��̌v�Z

	XMMATRIX matProjection =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//�㉺��p45�x
			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
			0.1f, 1000.0f
		);//�O�[�A���[

#pragma region �r���[�s��̍쐬
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);	//���_���W
	XMFLOAT3 target(0, 0, 0);	//�����_���W
	XMFLOAT3 up(0, 1, 0);		//������x�N�g��
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
		XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

#pragma endregion

#pragma region ���[���h�ϊ��s��

	//XMMATRIX matWorld;
	//matWorld = XMMatrixIdentity();

	//XMMATRIX matScale; //�X�P�[�����O�s��

	//XMMATRIX matRot; //��]�s��
	//matRot = XMMatrixIdentity();

	//XMMATRIX matTrans; //���s�ړ��s��
	//matTrans = XMMatrixTranslation(0, 0, 0);

	//matWorld *= matTrans; //���[���h�s��ɕ��s�ړ��𔽉f

#pragma endregion

#pragma endregion

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
//	for (int i = 0; i < _countof(indices); i++)
//	{
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
//	const wchar_t* texImgs[kTextureCount] =
//	{
//		L"Resources/texture.png",
//		L"Resources/reimu.png",
//	};
//
//	for (size_t i = 0; i < _countof(textureDatas); i++)
//	{
//		InitializeTexture(&textureDatas[i], texImgs[i]);
//	}
//
//	for (size_t i = 0; i < _countof(textureDatas); i++)
//	{
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
	//------�`�揉�������� �����܂�------
	//�Q�[�����[�v
	while (true) {
		//���͍X�V
		input->Update();
		
#pragma region �^�[�Q�b�g�̎�������J����
		if (input->ifKeyPress(DIK_D) || input->ifKeyPress(DIK_A))
		{
			if (input->ifKeyPress(DIK_D)) { angle += XMConvertToRadians(1.0f); }
			else if (input->ifKeyPress(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

			//angle���W�A������Y������ɉ�]�A���a��-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
				XMLoadFloat3(&target), XMLoadFloat3(&up));

			object3ds[0].constMapTransform->mat = matView * matProjection;
		}
#pragma endregion

#pragma region �A���ړ�

		for (size_t i = 0; i < _countof(object3ds); i++)
		{
			UpdateObject3d(&object3ds[i], matView, matProjection);
	
		}

		UpdateObjectControll(&object3ds[0], input);

		//�`��̏���
		dXBas->PrepareDraw();

#pragma region �@�����v�Z
		for (int i = 0; i < _countof(indices) / 3; i++)
		{
			//�O�p�`�P���ƂɌv�Z���Ă���
			//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
			unsigned short index0 = indices[i * 3 + 0];
			unsigned short index1 = indices[i * 3 + 1];
			unsigned short index2 = indices[i * 3 + 2];

			//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
			XMVECTOR p0 = XMLoadFloat3(&vertices[index0].pos);
			XMVECTOR p1 = XMLoadFloat3(&vertices[index1].pos);
			XMVECTOR p2 = XMLoadFloat3(&vertices[index2].pos);

			//p0->p1�x�N�g���A p0->p2�x�N�g�����v�Z (�x�N�g���̌��Z)
			XMVECTOR v1 = XMVectorSubtract(p1, p0);
			XMVECTOR v2 = XMVectorSubtract(p2, p0);

			//�O�ς͗������琂���ȃx�N�g��
			XMVECTOR normal = XMVector3Cross(v1, v2);

			//���K�� (������1�ɂ���)
			normal = XMVector3Normalize(normal);

			//���߂��@���𒸓_�f�[�^�ɑ��
			XMStoreFloat3(&vertices[index0].normal, normal);
			XMStoreFloat3(&vertices[index1].normal, normal);
			XMStoreFloat3(&vertices[index2].normal, normal);
		}
#pragma endregion

#pragma region �]��
		//�S���_�ɑ΂���
		for (int i = 0; i < _countof(vertices); i++)
		{
			vertMap[i] = vertices[i];//���W���R�s�[
		}
#pragma endregion

		//�R�}���h���X�g�̏�������C�Ɏ��s
		drawer->Update();

		//�S�I�u�W�F�N�g�ɂ��ď���
		for (int i = 0; i < _countof(object3ds); i++)
		{
			DrawObject3d(&object3ds[i], dXBas->GetCommandList(), vbView, ibView, _countof(indices));
		}

		//�`��㏈��
		dXBas->PostDraw();

		//windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage()) {
			//���[�v�𔲂���
			break;
		}
	}

	//WindowsAPI�I������
	winApp->Finalize();

	//�`��n�̉��
	delete drawer;
	drawer = nullptr;

	//��Ղ̉��
	delete dXBas;
	dXBas = nullptr;

	//���͂̉��
	delete input;
	input = nullptr;

	//WinApp�̉��
	delete winApp;
	winApp = nullptr;

	return 0;
}