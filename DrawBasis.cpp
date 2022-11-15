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

void DrawBasis::Initialize(DirectXBasis* dXBas) {
	assert(dXBas);
	dXBas_ = dXBas;

	//���_�o�b�t�@�r���[�̍쐬
	CreateVertexBufferView(dXBas_);

	//�V�F�[�_�t�@�C����ǂݍ��݁A�R���p�C������
	CompileShaderFile();

	//���_���C�A�E�g��g�ݗ��Ă�
	AssembleVertexLayout();

	//�O���t�B�b�N�X�p�C�v���C���𐶐�
	CreateGraphicsPipeline(dXBas_);
}

void DrawBasis::CreateVertexBufferView(DirectXBasis* dXBas) {
	HRESULT result;
	//���_�f�[�^
	Vector3 vertices[VerticesNum];

	//���_����
	typedef enum VerticesParts {
		LeftBottom,
		LeftTop,
		RightBottom,
	}VerticesParts;

	//�e�����ʒu�֌W
	float left = -5.0f;
	float right = +5.0f;
	float top = +5.0f;
	float bottom = -5.0f;

	//�e���ʂɁA�����ʒu�֌W��ݒ�
	vertices[LeftBottom] = Vector3(left, bottom, 0);
	vertices[LeftTop] = Vector3(left, top, 0);
	vertices[RightBottom] = Vector3(right, bottom, 0);

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
	result = dXBas->GetDevice()->CreateCommittedResource(
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
	//GPU���z�A�h���X
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView_.SizeInBytes = sizeVB;
	//���_�P���̃f�[�^�T�C�Y
	vbView_.StrideInBytes = sizeof(Vector3);
}

void DrawBasis::CompileShaderFile() {
	HRESULT result;

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/SpriteVS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		"main", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�\���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
		0,
		&vsBlob_, &errorBlob_);

	//�G���[�Ȃ�
	if (FAILED(result)) {
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//�s�N�Z���V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/SpritePS.hlsl",//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		"main", "ps_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�O�p�ݒ�
		0,
		&psBlob_, &errorBlob_);

	//�G���[�Ȃ�
	if (FAILED(result)) {
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob_->GetBufferSize());

		std::copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
}

void DrawBasis::AssembleVertexLayout() {
	//�v�f��
	typedef enum ElementName {
		Position,//���W
	}ElementName;

	//���_���C�A�E�g
	inputLayout_[ElementName::Position] = {
		//xyz���W
		"POSITION",									//�Z�}���e�B�b�N��
		0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł悢)
		DXGI_FORMAT_R32G32B32_FLOAT,				//�v�f���ƃr�b�g����\���@(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT
		0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
		D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l�@(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^��ʁ@(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
		0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
	};
}

void DrawBasis::CreateGraphicsPipeline(DirectXBasis* dXBas) {
	HRESULT result;

	//�O���t�B�b�N�X�p�C�v���C���f�X�N�̒��g��ݒ�
	SettingGraphicsPipelineDesc();

	//���[�g�V�O�l�`���𐶐�
	CreateRootSignature(dXBas_);

	//�p�C�v���C���X�e�[�g�̐���
	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void DrawBasis::SettingGraphicsPipelineDesc() {
	//�V�F�[�_�[�̐ݒ�
	pipelineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�

	//���X�^���C�U�̐ݒ�
	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
	pipelineDesc_.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����

	//�u�����h�X�e�[�g
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc_.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGB�S�Ẵ`���l����`��

	//�A���t�@�l���ʐݒ�
	blenddesc.BlendEnable = true; // �u�����h�L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; //�u�����h��L���ɂ���
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE; //���Z
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO; //�f�X�g�̒l�� 0%�g���@

	//���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��

	//���Z����
	//blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT; //���Z
	//blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l��100%�g��
	//blenddesc.DestBlend = D3D12_BLEND_ONE; //�f�X�g�̒l��100%�g��

	//�F���]
	//blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	//blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR; //1.0f-�f�X�g����|�̒l
	//blenddesc.DestBlend = D3D12_BLEND_ZERO; //�g��Ȃ�

	//����������
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD; //���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; //�\�[�X�̒l���A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_SRC_ALPHA; //1.0f-�\�[�X�̃A���t�@�l


//���_���C�A�E�g�̐ݒ�
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout_;
	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout_);

	//�}�`�̌`��ݒ�
	pipelineDesc_.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑��̐ݒ�
	pipelineDesc_.NumRenderTargets = 1;//�`��Ώۂ�1��
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`255�w���RGBA
	pipelineDesc_.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1��T���v�����O
}

void DrawBasis::SettingRootParameter() {
	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParam.Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParam.Descriptor.RegisterSpace = 0;						//�f�t�H���g
	rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
}

void DrawBasis::CreateRootSignature(DirectXBasis* dXBas) {
	HRESULT result;

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	//���[�g�V�O�l�`���̐���
	result = dXBas->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�C�`�����Z�b�g
	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void DrawBasis::CreateConstBuffer() {
	HRESULT result;

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
											//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResDesc{};
	cbResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
	cbResDesc.Height = 1;
	cbResDesc.DepthOrArraySize = 1;
	cbResDesc.MipLevels = 1;
	cbResDesc.SampleDesc.Count = 1;
	cbResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* constBuffMaterial = nullptr;

	//�萔�o�b�t�@�̐���
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&cbHeapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	result = constBuffMaterial->Map(0, nullptr,
		(void**)&constMapMaterial); //�}�b�s���O
	assert(SUCCEEDED(result));

	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = Vector4(1, 0, 0, 0.5f);
}

void DrawBasis::PrepareDraw() {
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g
}

void DrawBasis::PostDraw() {

}