#include <Windows.h>

#include "DrawBasis.h"
#include <DirectXMath.h>
#include "Vector3.h"
#include <wrl.h>

#include <d3d12.h>
#include <d3dcompiler.h>//�V�F�[�_�p�R���p�C��

#include <string>
#include <cassert>

#pragma comment(lib, "d3dcompiler.lib")//�V�F�[�_�p�R���p�C��

using namespace DirectX;

void DrawBasis::Initialize(DirectXBasis* dXBas){
	HRESULT result;

	LoadInstance(dXBas);

	CompileShaderFile();

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			//xyz���W
			"POSITION",									//�Z�}���e�B�b�N��
			0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł悢)
			DXGI_FORMAT_R32G32B32_FLOAT,				//�v�f���ƃr�b�g����\���@(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT
			0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
			D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l�@(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^��ʁ@(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
			0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
		},
	};

	CreateGraphicsPopeline();
}

void DrawBasis::LoadInstance(DirectXBasis* dXBas){
	dXBas_ = dXBas;
}

void DrawBasis::CreateVertexBufferView(){
	HRESULT result;

	//���_�f�[�^
	Vector3 vertices[] = {
		//x		 y		z
		{-5.0f, -5.0f, -5.0f},//����
		{-5.0f,  5.0f, -5.0f},//����
		{ 5.0f, -5.0f, -5.0f},//�E��
	};

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));

	//���_�o�b�t�@�̐ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
										   //���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
	Vector3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices); i++) {
		vertMap[i] = vertices[i];//���W���R�s�[
	}

	//�q���������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_�P���̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(Vector3);
}

void DrawBasis::CompileShaderFile(){
	HRESULT result;

	ComPtr<ID3DBlob> errorBlob = nullptr;//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicVS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		"main", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�\���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
		0,
		&vsBlob_, &errorBlob);

	//�G���[�Ȃ�
	if (FAILED(result)) {
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"BasicPS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		"main", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
		0,
		&psBlob_, &errorBlob);

	//�G���[�Ȃ�
	if (FAILED(result)) {
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void DrawBasis::CreateGraphicsPopeline(){
	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob_->GetBufferSize();
}