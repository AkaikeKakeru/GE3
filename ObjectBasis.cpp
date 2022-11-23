#include "ObjectBasis.h"
#include <d3dcompiler.h>
#include <string>
#include <cassert>

template <class Type>
using ComPtr = Microsoft::WRL::ComPtr<Type>;

//ComPtr<ID3D12Device> ObjectBasis::device_ = nullptr;
//ComPtr<ID3D12GraphicsCommandList> ObjectBasis::cmdList_ = nullptr;

ID3D12Device* ObjectBasis::device_ = nullptr;
ID3D12GraphicsCommandList* ObjectBasis::cmdList_ = nullptr;

std::vector<ObjectBasis::Vertex> ObjectBasis::vertices_;
std::vector<unsigned short> ObjectBasis::indices_;

void ObjectBasis::Initialize(/*ComPtr<ID3D12Device>*/
	ID3D12Device* device) {
	assert(device);
	ObjectBasis::device_ = device;

	HRESULT result;

#pragma region �`���񏉊��ݒ�
	//	float angle = 0.0f; //�J�����̉�]�p

	//���_�f�[�^
	Vertex vert[] = {
		//x		 y		z		�@��	u	  v
		//�O
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
		{{ 5.0f, -5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 0.0f}},//�E��

													  //���				 
		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 1.0f}},//����
		{{-5.0f,  5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����

													  //��							
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
		{{-5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����
		{{-5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��

													  //�E							
		{{ 5.0f,  5.0f, -5.0f},	{},		{1.0f, 1.0f}},//�E��
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
		{{ 5.0f, -5.0f,  5.0f},	{},		{0.0f, 0.0f}},//����

													  //��							
		{{-5.0f, -5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
		{{ 5.0f, -5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
		{{-5.0f, -5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
		{{ 5.0f, -5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��

													  //��							
		{{-5.0f,  5.0f,  5.0f},	{},		{1.0f, 1.0f}},//�E��
		{{ 5.0f,  5.0f,  5.0f},	{},		{1.0f, 0.0f}},//�E��
		{{-5.0f,  5.0f, -5.0f},	{},		{0.0f, 1.0f}},//����
		{{ 5.0f,  5.0f, -5.0f},	{},		{0.0f, 0.0f}},//����
	};

	//�����o�ϐ��ɃR�s�[
	//std::copy(std::begin(vert), std::end(vert), vertices_);

	for (size_t i = 0; i < _countof(vert); i++) {
		vertices_.emplace_back(vert[i]);
	}

	//�S���_�ɑ΂���
	//for (size_t i = 0; i < vertices_.size(); i++) {
	//	vertices_[i] = vert[i];//���W���R�s�[
	//}

	//�C���f�b�N�X�f�[�^
	unsigned short indi[] = {
		//�O
		0,1,2,//���
		2,1,3,//���
			  //���
			  4,5,6,//�O��
			  6,5,7,//�l��
					//��
					8,9,10,//���
					10,9,11,//���
							//�E
							12,13,14,
							14,13,15,
							//��
							16,17,18,//���
							18,17,19,//���
									 //��
									 20,21,22,
									 22,21,23,
	};

	//�����o�ϐ��ɃR�s�[
	//std::copy(std::begin(indi), std::end(indi), indices_);
	for (size_t i = 0; i < _countof(indi); i++) {
		indices_.emplace_back(indi[i]);
	}
	//�S���_�ɑ΂���
	//for (size_t i = 0; i < indices_.size(); i++) {
	//	indices_[i] = indi[i];//���W���R�s�[
	//}


	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	//UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
	UINT sizeVB = static_cast<UINT>(sizeof(Vertex) * vertices_.size());

	//���_�o�b�t�@�̐ݒ�
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC resDesc_{};
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = device_->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	/* vertices�ɋL�� */
	//GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
	//Vertex* vertMap_ = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap_);
	//if (SUCCEEDED(result)) {
	//}
	assert(SUCCEEDED(result));
		std::copy(vertices_.begin(), vertices_.end(), vertMap_);
		//�q���������
		vertBuff->Unmap(0, nullptr);

	//�S���_�ɑ΂���
	//for (size_t i = 0; i < vertices_.size(); i++) {
	//	vertMap_[i] = vertices_[i];//���W���R�s�[
	//}

	//���_�o�b�t�@�r���[�̍쐬
	//D3D12_VERTEX_BUFFER_VIEW vbView_{};
	//GPU���z�A�h���X
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView_.SizeInBytes = sizeVB;
	//���_�P���̃f�[�^�T�C�Y
	vbView_.StrideInBytes = sizeof(vertices_[0]);


	// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	//UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
	UINT sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());


	// ���\�[�X�ݒ�
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeIB; // �C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@�̐���
	//ComPtr<ID3D12Resource> indexBuff_ = nullptr;
	result = device_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff_));

	//�@�C���f�b�N�X�o�b�t�@���}�b�s���O
	//unsigned short* indexMap_ = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap_);
	assert(SUCCEEDED(result));

		std::copy(indices_.begin(), indices_.end(), indexMap_);
		indexBuff_->Unmap(0, nullptr);

	// �S�C���f�b�N�X�ɑ΂���
	//for (int i = 0; i < indices_.size(); i++) {
	//	indexMap_[i] = indices_[i]; //�C���f�b�N�X���R�s�[
	//}
	//�}�b�s���O����

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	//D3D12_INDEX_BUFFER_VIEW ibView_{};
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;


	//ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	//ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	//ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/ObjectVS.hlsl",//�V�F�[�_�t�@�C����
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
		L"Resources/shaders/ObjectPS.hlsl",//�V�F�[�_�t�@�C����
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
		},//(1�s�ŏ������������₷������)

			{
				//�@���x�N�g��
				"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
				D3D12_APPEND_ALIGNED_ELEMENT,
				D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
			},

		//���W�ȊO�� �F�A�e�N�X�`��UI�Ȃǂ�n���ꍇ�͂���ɑ�����
			{
				//UV���W
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

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob_->GetBufferSize();

	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�
														//pipelineDesc.SampleMask = UINT_MAX;

														//���X�^���C�U�̐ݒ�
														//�w�ʃJ�����O���ݒ肷��
														//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//�w�ʃJ�����O����
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����

														//�u�����h�X�e�[�g
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB�S�Ẵ`���l����`��

									   //�����_�\�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//�A���t�@�l���ʐݒ�
	blenddesc.BlendEnable = false; // �u�����h�L���ɂ���
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
	blenddesc.SrcBlend = D3D12_BLEND_ONE; //�\�[�X�̒l���A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_ONE; //1.0f-�\�[�X�̃A���t�@�l

										   //���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//�}�`�̌`��ݒ�
	pipelineDesc.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

#pragma region �[�x�e�X�g�̐ݒ�
	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_R32_FLOAT;
#pragma endregion

	//���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;//�`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1��T���v�����O

									  //���[�g�V�O�l�`��
									  //ComPtr<ID3D12RootSignature> rootSignature_;

									  //�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;//��x�̕`��Ɏg���e�N�X�`�����ꖇ�Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;//�e�N�X�`�����W�X�^�ԍ�0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//�f�X�N���v�^�e�[�u���̐ݒ�
	D3D12_DESCRIPTOR_RANGE descRange{};
	descRange.NumDescriptors = 1;//�萔�͈��
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //��ʂ͒萔
	descRange.BaseShaderRegister = 0; //0�ԃX���b�g����
	descRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	////���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParams[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����

																	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//�萔�o�b�t�@�r���[
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;					//�萔�o�b�t�@�ԍ�
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�t�H���g�l
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����

																	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams[2].Descriptor.ShaderRegister = 1;					//�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����

																	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams; //���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams); //���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	//���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob_);
	assert(SUCCEEDED(result));

	result = device_->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�C�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//�p�C�v���C���X�e�[�g�̐���
	//ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	result = device_->CreateGraphicsPipelineState(&pipelineDesc,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

	//ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;

#pragma endregion

#pragma region constMapMaterial�֘A
	{

		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbheapprop{};
		cbheapprop.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
												  //���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbresdesc{};
		cbresdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbresdesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
		cbresdesc.Height = 1;
		cbresdesc.DepthOrArraySize = 1;
		cbresdesc.MipLevels = 1;
		cbresdesc.SampleDesc.Count = 1;
		cbresdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		//�萔�o�b�t�@�̐���
		result = device_->CreateCommittedResource(
			&cbheapprop, //�q�[�v�ݒ�
			D3D12_HEAP_FLAG_NONE,
			&cbresdesc, //���\�[�X�ݒ�
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuffMaterial_));
		assert(SUCCEEDED(result));

		//�萔�o�b�t�@�̃}�b�s���O
		//ConstBufferDataMaterial* constMapMaterial_ = nullptr;
		result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial_); //�}�b�s���O

		// �l���������ނƎ����I�ɓ]�������
		constMapMaterial_->color = Vector4(0.5f, 0.5f, 0.5f, 1.0f); //RGBA�Ŕ������̐�

		//�}�b�s���O����
		constBuffMaterial_->Unmap(0, nullptr);
		assert(SUCCEEDED(result));

	}
#pragma endregion

#pragma region constMapTransfrom�֘A
	{

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
		//const size_t kObjectCount = 1;
		//3D�I�u�W�F�N�g�̔z��
		//ObjectBasis object3ds[kObjectCount];

		Vector3 rndScale;
		Vector3 rndRota;
		Vector3 rndPos;

		//������
		//Initialize(/*dXBas_*/);

#pragma region constMapTransfrom�֘A
		{

			//�q�[�v�ݒ�
			D3D12_HEAP_PROPERTIES heapProp{};
			heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
													//���\�[�X�ݒ�
			//D3D12_RESOURCE_DESC resDesc{};
			resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resDesc_.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
			resDesc_.Height = 1;
			resDesc_.DepthOrArraySize = 1;
			resDesc_.MipLevels = 1;
			resDesc_.SampleDesc.Count = 1;
			resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

			//�萔�o�b�t�@�̐���
			result = device_->CreateCommittedResource(
				&heapProp, //�q�[�v�ݒ�
				D3D12_HEAP_FLAG_NONE,
				&resDesc_, //���\�[�X�ݒ�
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&constBuffTransform_));
			assert(SUCCEEDED(result));

			//�萔�o�b�t�@�̃}�b�s���O
			result = constBuffTransform_->Map(0, nullptr,
				(void**)&constMapTransform_); //�}�b�s���O
			assert(SUCCEEDED(result));
		}
#pragma endregion

#pragma endregion

#pragma region �P�ʍs��Ŗ��߂���
#pragma region ���s���e�s��̌v�Z

		////DirectXMath�ŗp�ӂ���Ă���֐��ɒu������
		//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
		//	0.0f, window_width,//���[�A�E�[
		//	window_height, 0.0f,//���[�A��[
		//	0.0f, 1.0f);//�O�[�A���[
#pragma endregion

#pragma region �r���[�v���g�p
	////�r���[�v���W�F�N�V����������
	//viewPro_->Initialize();

	////�z����̑S�I�u�W�F�N�g�ɑ΂���
	//viewPro_->GetViewProjection() = viewPro_->GetViewProjection().matPro_;
	//viewPro_->view = viewPro_->GetViewProjection().matView_;
#pragma endregion

#pragma endregion


	//const int kTextureCount = 2;
	//TextureData textureDatas[kTextureCount] = { 0 };

		const wchar_t* texImgs = {
			L"Resources/smile.png"
		};

		InitializeTexture(texImgs);

		TransferTextureBuffer();

		//���f�[�^�J��
		//delete[] imageData;

		//SRV�̍ő��
		const size_t kMaxSRVCount = 2056;

		//�f�X�N���v�^�q�[�v�̐ݒ�
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
		srvHeapDesc.NumDescriptors = kMaxSRVCount;

		//�ݒ�����SRV�p�f�X�N���v�^�q�[�v�𐶐�
		//ID3D12DescriptorHeap* srvHeap_ = nullptr;
		result = device_->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap_));
		assert(SUCCEEDED(result));

		//SRV�q�[�v�̐擪�n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap_->GetCPUDescriptorHandleForHeapStart();

		//�V�F�[�_���\�[�X�r���[�ݒ�
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
		srvDesc.Format = resDesc_.Format;//RGBA float
		srvDesc.Shader4ComponentMapping =
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		srvDesc.Texture2D.MipLevels = resDesc_.MipLevels;

		//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
		device_->CreateShaderResourceView(textureData_.texBuff.Get(), &srvDesc, srvHandle);

#pragma region �e�N�X�`���̍����ւ��ŒǋL
		//�T�C�Y�ύX
		UINT incrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		srvHandle.ptr += incrementSize;

		////2���ڗp
		////�V�F�[�_���\�[�X�r���[�ݒ�
		//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//�ݒ�\����
		//srvDesc2.Format = textureDatas[1].textureResourceDesc.Format;//RGBA float
		//srvDesc2.Shader4ComponentMapping =
		//	D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		//srvDesc2.Texture2D.MipLevels = textureDatas[1].textureResourceDesc.MipLevels;

		////�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
		//device_->CreateShaderResourceView(textureDatas[1].texBuff.Get(), &srvDesc2, srvHandle);

#pragma endregion

	//CBV,SRV,UAV��1���̃T�C�Y���擾
		UINT descriptorSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//SRV�q�[�v�̐擪�n���h�����擾
		//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
		//�n���h������i�߂�(SRV�̈ʒu)
		srvHandle.ptr += descriptorSize * 1;

		//CBV(�R���X�^���g�o�b�t�@�r���[)�̐ݒ�
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
		//cbvDesc�̒l�ݒ�(�ȗ�)
		device_->CreateConstantBufferView(&cbvDesc, srvHandle);

	}
#pragma endregion

#pragma endregion
}

void ObjectBasis::InitializeTexture(const wchar_t* szFile) {
	HRESULT result;

	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		szFile,
		WIC_FLAGS_NONE,
		&textureData_.metadata,
		textureData_.scratchImg);

	//�~�b�v�}�b�v����
	result = GenerateMipMaps(
		textureData_.scratchImg.GetImages(),
		textureData_.scratchImg.GetImageCount(),
		textureData_.scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, textureData_.mipChine);
	if (SUCCEEDED(result)) {
		textureData_.scratchImg = std::move(textureData_.mipChine);
		textureData_.metadata = textureData_.scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾂Ńf�C�q���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	textureData_.metadata.format = MakeSRGB(textureData_.metadata.format);
};

void ObjectBasis::TransferTextureBuffer() {
	HRESULT result;

	//�q�[�v�ݒ�
	//D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureData_.textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData_.textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData_.textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC textureResourceDesc{};
	textureData_.textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData_.textureResourceDesc.Format = textureData_.metadata.format;
	textureData_.textureResourceDesc.Width = textureData_.metadata.width; //��
	textureData_.textureResourceDesc.Height = (UINT)textureData_.metadata.height; //����
	textureData_.textureResourceDesc.DepthOrArraySize = (UINT16)textureData_.metadata.arraySize;
	textureData_.textureResourceDesc.MipLevels = (UINT16)textureData_.metadata.mipLevels;
	textureData_.textureResourceDesc.SampleDesc.Count = 1;

	////�e�N�X�`���o�b�t�@�̐���
	//ID3D12Resource* texBuff = nullptr;

	result = device_->CreateCommittedResource(
		&textureData_.textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData_.textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData_.texBuff));

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < textureData_.metadata.mipLevels; i++) {
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = textureData_.scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = textureData_.texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			(UINT)img->rowPitch,//1���C���T�C�Y
			(UINT)img->slicePitch//�ꖇ�T�C�Y
		);
		assert(SUCCEEDED(result));
	}
}

//�I�u�W�F�N�g�X�V����
void  ObjectBasis::Update() {
	//Matrix4 matScale, matRota, matTrans, matX, matY, matZ;

	////�X�P�[���A��]�A���s�ړ��s��̌v�Z
	//matScale = Mat4Scale(worldTransform_.scale_);
	//matRota = Mat4Identity();

	//matZ = Mat4RotationZ(worldTransform_.rotation_.z);
	//matX = Mat4RotationX(worldTransform_.rotation_.x);
	//matY = Mat4RotationY(worldTransform_.rotation_.y);

	//matRota = matRota * matZ * matX * matY;

	//matTrans = Mat4Translation(worldTransform_.position_);

	////���[���h�s��̍���
	//worldTransform_.matWorld_ = Mat4Identity();	//�ό`���Z�b�g

	//worldTransform_.matWorld_ *= matScale;	//���[���h�s��̃X�P�[�����O�𔽉f
	//worldTransform_.matWorld_ *= matRota;	//���[���h�s��ɉ�]�𔽉f
	//worldTransform_.matWorld_ *= matTrans;	//���[���h�s��ɕ��s�ړ��𔽉f

	worldTransform_.RecalculationMatWorld();

	//�e�I�u�W�F�N�g�������
	if (parent_ != nullptr) {
		//�e�I�u�W�F�N�g�̃��[���h�s����|����
		worldTransform_.matWorld_ *= parent_->worldTransform_.matWorld_;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	constMapTransform_->mat = worldTransform_.matWorld_ *
		viewPro_->GetViewProjection().matView_ *
		viewPro_->GetViewProjection().matPro_;
}

void  ObjectBasis::Draw(/*UINT numIndices*/) {
	////���_�o�b�t�@�̐ݒ�
	//dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);
	////�C���f�b�N�X�o�b�t�@�̐ݒ�
	//dXBas_->GetCommandList()->IASetIndexBuffer(&ibView_);
	////�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	//dXBas_->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());

	////�`��R�}���h
	//dXBas_->GetCommandList()->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);

	assert(device_);
	assert(ObjectBasis::cmdList_);

	//���_�o�b�t�@�̐ݒ�
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	cmdList_->IASetIndexBuffer(&ibView_);

	//SRV�q�[�v�̐擪�n���h�����擾(SRV���w���Ă���͂�)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	cmdList_->SetGraphicsRootConstantBufferView(0, constBuffMaterial_->GetGPUVirtualAddress());

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	cmdList_->SetGraphicsRootConstantBufferView(2, constBuffTransform_->GetGPUVirtualAddress());
	//SRV�q�[�v�̐ݒ�R�}���h
	cmdList_->SetDescriptorHeaps(1, &srvHeap_);

	cmdList_->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	//�`��R�}���h
	cmdList_->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);


}

void ObjectBasis::copyInit(DirectXBasis* dXBas) {

}

void  ObjectBasis::copyUp() {
}

void ObjectBasis::copyDraw() {

#pragma region �@�����v�Z
	for (int i = 0; i < indices_.size() / 3; i++) {
		//�O�p�`�P���ƂɌv�Z���Ă���
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short index0 = indices_[static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(i) * 3 + 0];
		unsigned short index1 = indices_[static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(i) * 3 + 1];
		unsigned short index2 = indices_[static_cast<std::vector<unsigned short, std::allocator<unsigned short>>::size_type>(i) * 3 + 2];

		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		Vector3 p0 = vertices_[index0].pos;
		Vector3 p1 = vertices_[index1].pos;
		Vector3 p2 = vertices_[index2].pos;

		//p0->p1�x�N�g���A p0->p2�x�N�g�����v�Z (�x�N�g���̌��Z)
		Vector3 v1 = p1 - p0;
		Vector3 v2 = p2 - p0;

		//�O�ς͗������琂���ȃx�N�g��
		Vector3 normal = Vec3Cross(v1, v2);

		//���K�� (������1�ɂ���)
		normal = Vec3Normalize(normal);

		//���߂��@���𒸓_�f�[�^�ɑ��
		vertices_[index0].normal = normal;
		vertices_[index1].normal = normal;
		vertices_[index2].normal = normal;
	}
#pragma endregion

#pragma region �]��
	//�S���_�ɑ΂���
	for (int i = 0; i < vertices_.size(); i++) {
		vertMap_[i] = vertices_[i];//���W���R�s�[
	}

	//result = vertBuff->Map(0, nullptr, (void**)&vertMap_);
	//assert(SUCCEEDED(result));
	//std::copy(vertices_.begin(), vertices_.end(), vertMap_);
#pragma endregion
}

void ObjectBasis::PrepareDraw(/*ComPtr<ID3D12GraphicsCommandList>*/
	ID3D12GraphicsCommandList* cmdList) {
	/*assert(dXBas_->GetCommandList());*/
	assert(ObjectBasis::cmdList_ == nullptr);

	ObjectBasis::cmdList_ = cmdList;

	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	cmdList->SetPipelineState(pipelineState_.Get());
	cmdList->SetGraphicsRootSignature(rootSignature_.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g
}

void ObjectBasis::PostDraw() {
	//�R�}���h���X�g����
	ObjectBasis::cmdList_ = nullptr;
}