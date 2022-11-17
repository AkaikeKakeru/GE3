#include <Windows.h>

#include "DrawBasis.h"
#include <DirectXMath.h>
#include "Vector3.h"
#include "Vector2.h"
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

	//���_�f�[�^�\����
	struct Vertex {
		Vector3 pos;//xyz���W
		Vector2 uv;//uv���W
	};

	//���_�f�[�^
	Vertex vertices[VerticesNum];

	//���_����
	typedef enum VerticesParts {
		LeftBottom,//����
		LeftTop,//����
		RightBottom,//�E��
		RightTop,//�E��
	}VerticesParts;

	//�e�����ʒu�֌W
	/*�A�X�y�N�g����l�����������Ő����`���ۂ�����*/
	float left = -0.4f;//��
	float right = +0.4f;//�E
	float top = +0.7f;//��
	float bottom = -0.7f;//��

	/*�����^�C�v*/
	//float left = -0.5f;//��
	//float right = +0.5f;//�E
	//float top = +0.5f;//��
	//float bottom = -0.5f;//��

	float uvLeft = 0.0f;//uv��
	float uvRight = 1.0f;//uv�E
	float uvTop = 0.0f;//uv��
	float uvBottom = 1.0f;//uv��

	//�e���ʂɁA�����ʒu�֌W��ݒ�
	vertices[LeftBottom].pos = Vector3(left, bottom, 0);
	vertices[LeftTop].pos = Vector3(left, top, 0);
	vertices[RightBottom].pos = Vector3(right, bottom, 0);
	vertices[RightTop].pos = Vector3(right, top, 0);

	vertices[LeftBottom].uv = Vector2(uvLeft, uvBottom);
	vertices[LeftTop].uv = Vector2(uvLeft, uvTop);
	vertices[RightBottom].uv = Vector2(uvRight, uvBottom);
	vertices[RightTop].uv = Vector2(uvRight, uvTop);

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));

	//���_�o�b�t�@�̐ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
	//���\�[�X�ݒ�
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	result = dXBas->GetDevice()->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
	Vertex* vertMap = nullptr;
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
	vbView_.StrideInBytes = sizeof(vertices[0]);
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
		Position,//xyz���W
		Texcoord,//uv���W
	}ElementName;

	//���_���C�A�E�g
	for (int i = 0; i < ElementDescNum; i++) {
		switch (i) {
		case ElementName::Position:
			inputLayout_[i] = {
				//xyz���W
				"POSITION",									//�Z�}���e�B�b�N��
				0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł悢)
				DXGI_FORMAT_R32G32B32_FLOAT,				//�v�f���ƃr�b�g����\���@(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT
				0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
				D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l�@(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^��ʁ@(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
				0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
			};
			break;

		case ElementName::Texcoord:
			inputLayout_[i] = {
				//uv���W
				"TEXCOORD",									//�Z�}���e�B�b�N��
				0,											//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł悢)
				DXGI_FORMAT_R32G32_FLOAT,				//�v�f���ƃr�b�g����\���@(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT
				0,											//���̓X���b�g�C���f�b�N�X(0�ł悢)
				D3D12_APPEND_ALIGNED_ELEMENT,				//�f�[�^�̃I�t�Z�b�g�l�@(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	//���̓f�[�^��ʁ@(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DAT
				0											//��x�ɕ`�悷��C���X�^���X��(0�ł悢)
			};
			break;

		default:
			break;
		}
	}
}

void DrawBasis::CreateGraphicsPipeline(DirectXBasis* dXBas) {
	HRESULT result;

	//�O���t�B�b�N�X�p�C�v���C���f�X�N�̒��g��ݒ�
	SettingGraphicsPipelineDesc();

	//�f�X�N���v�^�����W�ݒ�
	SettingDescriptorRange();

	//���[�g�p�����[�^�ݒ�
	SettingRootParameter();

	//�e�N�X�`���T���v���[�ݒ�
	SettingTextureSampler();

	//���[�g�V�O�l�`���𐶐�
	CreateRootSignature(dXBas_);

	//�p�C�v���C���X�e�[�g�̐���
	result = dXBas->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@����
	CreateConstBuffer();

	//�e�N�X�`��������
	initializeTexture();

	//�f�X�N���v�^�q�[�v����
	CreateDescriptorHeap();

	//�V�F�[�_���\�[�X�r���[����
	CreateShagerResourceView();
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
	//�萔�o�b�t�@0��
	rootParams_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParams_[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams_[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g
	rootParams_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//�f�X�N���v�^�e�[�u��
	rootParams_[1].DescriptorTable.pDescriptorRanges = &descriptorRange_;				//�f�X�N���v�^�����W
	rootParams_[1].DescriptorTable.NumDescriptorRanges = 1;					//�f�X�N���v�^�����W��
	rootParams_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
}

void DrawBasis::CreateRootSignature(DirectXBasis* dXBas) {
	HRESULT result;

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	/*���[�g�p�����[�^���*/
	rootSignatureDesc.pParameters = rootParams_;//���[�g�p�����[�^�擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams_);//���[�g�p�����[�^��
	/*�e�N�X�`���T���v���[���*/
	rootSignatureDesc.pStaticSamplers = &samplerDesc_;
	rootSignatureDesc.NumStaticSamplers = 1;

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

	//�萔�o�b�t�@�̐���
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&cbHeapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffMaterial_->Map(0, nullptr,
		(void**)&constMapMaterial_); //�}�b�s���O
	assert(SUCCEEDED(result));

	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial_->color = Vector4(1, 0, 0, 0.5f);
}

void DrawBasis::initializeTexture() {
	HRESULT result;

	/////������
	//imageData_ = new Vector4[imageDataCount]; //���K���J������

	////�S�s�N�Z���̐F��������
	//for (size_t i = 0; i < imageDataCount; i++) {
	//	imageData_[i].x = 1.0f;	//R
	//	imageData_[i].y = 0.0f;	//G
	//	imageData_[i].z = 0.0f;	//B
	//	imageData_[i].w = 1.0f;	//A
	//}

	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/smile.png",
		WIC_FLAGS_NONE,
		&metadata_, scratchImg_);

	//�~�b�v�}�b�v����
	ScratchImage mipChain{};
	result = GenerateMipMaps(
		scratchImg_.GetImages(), scratchImg_.GetImageCount(), scratchImg_.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg_ = std::move(mipChain);
		metadata_ = scratchImg_.GetMetadata();
	}

	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata_.format = MakeSRGB(metadata_.format);

	///�e�N�X�`���o�b�t�@
	//�e�N�X�`���o�b�t�@����
	CreateTextureBuffer();
	//�e�N�X�`���o�b�t�@�]��
	TransferTextureBuffer();

	//���f�[�^���
	delete[] imageData_;
	imageData_ = nullptr;
}

void DrawBasis::CreateTextureBuffer() {
	HRESULT result;

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata_.format;
	textureResourceDesc.Width = metadata_.width; //��
	textureResourceDesc.Height = (UINT)metadata_.height; //����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata_.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata_.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	result = dXBas_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff_));
}

void DrawBasis::TransferTextureBuffer() {
	HRESULT result;

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuff_->WriteToSubresource(
		0,
		nullptr,//�S�̈�փR�s�[
		imageData_,//���f�[�^�A�h���X
		static_cast<UINT>(sizeof(Vector4) * textureWidth),//1���C���T�C�Y
		static_cast<UINT>(sizeof(Vector4) * imageDataCount)//�ꖇ�T�C�Y
	);
	assert(SUCCEEDED(result));
}

void DrawBasis::CreateDescriptorHeap() {
	HRESULT result;

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	srvHeapDesc.NumDescriptors = static_cast<UINT>(kMaxSRVCount);

	//�ݒ�����SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dXBas_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));

	//SRV�q�[�v�̐擪�n���h�����擾
	srvHandle_ = srvHeap_->GetCPUDescriptorHandleForHeapStart();
}

void DrawBasis::CreateShagerResourceView() {
	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = resDesc_.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc_.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	dXBas_->GetDevice()->CreateShaderResourceView(texBuff_.Get(), &srvDesc, srvHandle_);
}

void DrawBasis::SettingDescriptorRange() {
	//�f�X�N���v�^�����W�̐ݒ�
	descriptorRange_.NumDescriptors = 1;//��x�̕`��Ɏg���e�N�X�`�����ꖇ�Ȃ̂�1
	descriptorRange_.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_.BaseShaderRegister = 0;//�e�N�X�`�����W�X�^�ԍ�0��
	descriptorRange_.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void DrawBasis::SettingTextureSampler(){
	//�e�N�X�`���T���v���[�̐ݒ�
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

void DrawBasis::PrepareDraw() {
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	//dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);//�O�p�`���X�g

	// �萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetGraphicsRootConstantBufferView(
		0, constBuffMaterial_->GetGPUVirtualAddress());

	//SRV�q�[�v�̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetDescriptorHeaps(1, &srvHeap_);

	//SRV�q�[�v�̐擪�n���h�����擾(SRV���w���Ă���͂�)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();

	//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	dXBas_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
}

void DrawBasis::PostDraw() {

}