#include "Drawer.h"
#include <cassert>
#include <d3dcompiler.h>
#include <string>

void Drawer::Initialize(DirectXBasis* dXBas,
	const wchar_t* vsFile,const wchar_t* psFile){
	assert(dXBas);
	dXBas_ = dXBas;

	LoadShaderFile(vsFile,psFile);

	VertexBufferInitialize();

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	SetingGraphicsPipeline();

	//�萔�o�b�t�@����
	CreateConstBuffer();
#pragma region constMapMaterial�֘A

#pragma endregion

	TextureInitialize();
}

void Drawer::VertexBufferInitialize(){
	HRESULT result;
	
	//���_�f�[�^
	vertices_ =
	{
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


	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * /*_countof(*/vertices_.size());

	//���_�o�b�t�@�̐ݒ�
	//�q�[�v�ݒ�
	heapProp_.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�̓]���p
	//���\�[�X�ݒ�
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&heapProp_,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ����z������(���C����������)���擾
	
	result = vertBuff->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));

	/* vertices�ɋL�� */

	//�S���_�ɑ΂���
	for (int i = 0; i < vertices_.size(); i++)
	{
		vertMap_[i] = vertices_[i];//���W���R�s�[
	}

	//�q���������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬;
	//GPU���z�A�h���X
	vbView_.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView_.SizeInBytes = sizeVB;
	//���_�P���̃f�[�^�T�C�Y
	vbView_.StrideInBytes = sizeof(vertices_[0]);

}

void Drawer::TextureInitialize(){
	HRESULT result;

	//�C���f�b�N�X�f�[�^
	indices_ =
	{
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


	// �C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * /*_countof(*/indices_.size())/*)*/;

	// ���\�[�X�ݒ�
	resDesc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc_.Width = sizeIB; // �C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc_.Height = 1;
	resDesc_.DepthOrArraySize = 1;
	resDesc_.MipLevels = 1;
	resDesc_.SampleDesc.Count = 1;
	resDesc_.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@�̐���
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&heapProp_,
		D3D12_HEAP_FLAG_NONE,
		&resDesc_,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));

	//�@�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	// �S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < /*_countof(*/indices_.size()/*)*/; i++)
	{
		indexMap[i] = indices_[i]; //�C���f�b�N�X���R�s�[
	}
	//�}�b�s���O����
	indexBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	
	ibView_.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	const int kTextureCount = 2;
	TextureData textureDatas[kTextureCount] = { 0 };

	const wchar_t* texImgs[kTextureCount] =
	{
		L"Resources/texture.png",
		L"Resources/reimu.png",
	};

	for (size_t i = 0; i < _countof(textureDatas); i++)
	{
		InitializeTexture(&textureDatas[i], texImgs[i]);
	}

	for (size_t i = 0; i < _countof(textureDatas); i++)
	{
		TransferTextureBuffer(&textureDatas[i], dXBas_->GetDevice());
	}

	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc_ = {};
	srvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	srvHeapDesc_.NumDescriptors = kMaxSRVCount;

	//�ݒ�����SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dXBas_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc_, IID_PPV_ARGS(&srvHeap_));
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
	dXBas_->GetDevice()->CreateShaderResourceView(textureDatas[0].texBuff.Get(), &srvDesc, srvHandle);

#pragma region �e�N�X�`���̍����ւ��ŒǋL
	//�T�C�Y�ύX
	incrementSize_ = dXBas_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize_;

	//2���ڗp
	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//�ݒ�\����
	srvDesc2.Format = textureDatas[1].textureResourceDesc.Format;//RGBA float
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc2.Texture2D.MipLevels = textureDatas[1].textureResourceDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	dXBas_->GetDevice()->CreateShaderResourceView(textureDatas[1].texBuff.Get(), &srvDesc2, srvHandle);

#pragma endregion

	//CBV,SRV,UAV��1���̃T�C�Y���擾
	UINT descriptorSize = dXBas_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRV�q�[�v�̐擪�n���h�����擾
	//D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	//�n���h������i�߂�(SRV�̈ʒu)
	srvHandle.ptr += descriptorSize * 1;

	//CBV(�R���X�^���g�o�b�t�@�r���[)�̐ݒ�
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	//cbvDesc�̒l�ݒ�(�ȗ�)
	dXBas_->GetDevice()->CreateConstantBufferView(&cbvDesc, srvHandle);

#pragma endregion

#pragma endregion
}

void Drawer::Update(){
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetPipelineState(pipelineState_.Get());
	dXBas_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//�O�p�`���X�g

																						 //���_�o�b�t�@�r���[�̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetGraphicsRootConstantBufferView(0,constBuffMaterial_->GetGPUVirtualAddress());

	//SRV�q�[�v�̐ݒ�R�}���h
	dXBas_->GetCommandList()->SetDescriptorHeaps(1, &srvHeap_);

	//SRV�q�[�v�̐擪�n���h�����擾(SRV���w���Ă���͂�)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
	//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	dXBas_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);

	if (ifOneTextureNum_ == false)
	{
		//2���ڂ��w�������悤�ɂ���SRV�̃n���h�������[�g�p�����[�^�ɐݒ�
		srvGpuHandle.ptr += incrementSize_;
		dXBas_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	}

	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	dXBas_->GetCommandList()->IASetIndexBuffer(&ibView_);



}

//�O���t�B�b�N�X�p�C�v���C���ݒ�
void Drawer::SetingGraphicsPipeline(){
	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
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

	//�V�F�[�_�[�̐ݒ�
	SetingShader();
	//�T���v���}�X�N�̐ݒ�
	pipelineDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//�W���ݒ�
	//���X�^���C�U�̐ݒ�
	SetingRasterizer();
	//�A���t�@�u�����f�B���O�̐ݒ�
	SetingAlphaBlend();
	//���_���C�A�E�g�̐ݒ�
	pipelineDesc_.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc_.InputLayout.NumElements = _countof(inputLayout);
	//�}�`�̌`��ݒ�
	pipelineDesc_.PrimitiveTopologyType
		= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	SetingDepthStencilState();
	//���̑��̐ݒ�
	SetingOther();

	//�f�X�N���v�^�����W�̐ݒ�
	SetingDescriptorRange();
	//�f�X�N���v�^�e�[�u���̐ݒ�
	SetingDescriptorTable();

	//�e�N�X�`���T���v���[�̐ݒ�
	SetingTextureSampler();

	///���[�g�V�O�l�`���֘A
	//���[�g�p�����[�^�̐ݒ�
	SetingRootParameter();
	//���[�g�V�O�l�`���̐ݒ�
	SetingRootSignature();

	//�p�C�v���C���X�e�[�g����
	CreatePipelineState();
}


void Drawer::CreateConstBuffer(){
	CreateConstBufferMaterial();
}

//�O���t�B�b�N�X�p�C�v���C���ݒ�̒��g
void Drawer::LoadShaderFile(const wchar_t* vsFile,const wchar_t* psFile){
	HRESULT result;

	//���_�V�F�[�_�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		vsFile,//�V�F�[�_�t�@�C����
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
		psFile,//�V�F�[�_�t�@�C����
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
void Drawer::SetingShader(){
	pipelineDesc_.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	pipelineDesc_.VS.BytecodeLength = vsBlob_->GetBufferSize();
	pipelineDesc_.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	pipelineDesc_.PS.BytecodeLength = psBlob_->GetBufferSize();
}
void Drawer::SetingRasterizer(){
	//�w�ʃJ�����O���ݒ肷��
	//pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	pipelineDesc_.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//�w�ʃJ�����O����
	pipelineDesc_.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
	pipelineDesc_.RasterizerState.DepthClipEnable = true;//�[�x�N���b�s���O��L����
}
void Drawer::SetingAlphaBlend(){
	//�u�����h�X�e�[�g
	pipelineDesc_.BlendState.RenderTarget[0].RenderTargetWriteMask 
		= D3D12_COLOR_WRITE_ENABLE_ALL;//RGB�S�Ẵ`���l����`��

	//�����_�\�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc_.BlendState.RenderTarget[0];
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
}
void Drawer::SetingDepthStencilState(){
	pipelineDesc_.DepthStencilState.DepthEnable = true;
	pipelineDesc_.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc_.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc_.DSVFormat = DXGI_FORMAT_R32_FLOAT;
}
void Drawer::SetingOther(){
	pipelineDesc_.NumRenderTargets = 1;//�`��Ώۂ�1��
	pipelineDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`255�w���RGBA
	pipelineDesc_.SampleDesc.Count = 1;//1�s�N�Z���ɂ�1��T���v�����O
}

void Drawer::SetingDescriptorRange(){
	descriptorRange_.NumDescriptors = 1;//��x�̕`��Ɏg���e�N�X�`�����ꖇ�Ȃ̂�1
	descriptorRange_.RangeType = 
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_.BaseShaderRegister = 0;//�e�N�X�`�����W�X�^�ԍ�0��
	descriptorRange_.OffsetInDescriptorsFromTableStart = 
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void Drawer::SetingDescriptorTable(){
	descRange_.NumDescriptors = 1;//�萔�͈��
	descRange_.RangeType = 
		D3D12_DESCRIPTOR_RANGE_TYPE_CBV; //��ʂ͒萔
	descRange_.BaseShaderRegister = 0; //0�ԃX���b�g����
	descRange_.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
}

void Drawer::SetingTextureSampler(){
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

void Drawer::SetingRootParameter(){
	///���[�g�p�����[�^�̐ݒ�
	
	//�萔�o�b�t�@0��
	rootParams_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//�萔�o�b�t�@�r���[
	rootParams_[0].Descriptor.ShaderRegister = 0;					//�萔�o�b�t�@�ԍ�
	rootParams_[0].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����

	//�e�N�X�`�����W�X�^0��
	rootParams_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//�萔�o�b�t�@�r���[
	rootParams_[1].DescriptorTable.pDescriptorRanges = &descriptorRange_;					//�萔�o�b�t�@�ԍ�
	rootParams_[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�t�H���g�l
	rootParams_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����

	//�萔�o�b�t�@1��
	rootParams_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//���
	rootParams_[2].Descriptor.ShaderRegister = 1;					//�萔�o�b�t�@�ԍ�
	rootParams_[2].Descriptor.RegisterSpace = 0;						//�f�t�H���g�l
	rootParams_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//�S�ẴV�F�[�_���猩����
}
void Drawer::SetingRootSignature(){
	HRESULT result;

	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams_; //���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams_); //���[�g�p�����[�^��
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

	result = dXBas_->GetDevice()->CreateRootSignature(
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�C�`�����Z�b�g
	pipelineDesc_.pRootSignature = rootSignature_.Get();
}

void Drawer::CreatePipelineState(){
	HRESULT result;

	//�p�C�v���C���X�e�[�g�̐���
	result = dXBas_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc_,
		IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}

void Drawer::CreateConstBufferMaterial(){
	HRESULT result;

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
	result = dXBas_->GetDevice()->CreateCommittedResource(
		&cbheapprop, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbresdesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial_->Map(0, nullptr, (void**)&constMapMaterial); //�}�b�s���O

	// �l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f); //RGBA�Ŕ������̐�

	//�}�b�s���O����
	constBuffMaterial_->Unmap(0, nullptr);
	assert(SUCCEEDED(result));
}

void Drawer::TransferVertices(){
#pragma region �@�����v�Z
	for (int i = 0; i < indices_.size() / 3; i++)
	{
		//�O�p�`�P���ƂɌv�Z���Ă���
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short index0 = indices_[i * 3 + 0];
		unsigned short index1 = indices_[i * 3 + 1];
		unsigned short index2 = indices_[i * 3 + 2];

		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices_[index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices_[index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices_[index2].pos);

		//p0->p1�x�N�g���A p0->p2�x�N�g�����v�Z (�x�N�g���̌��Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);

		//���K�� (������1�ɂ���)
		normal = XMVector3Normalize(normal);

		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices_[index0].normal, normal);
		XMStoreFloat3(&vertices_[index1].normal, normal);
		XMStoreFloat3(&vertices_[index2].normal, normal);
	}
#pragma endregion

#pragma region �]��
	//�S���_�ɑ΂���
	for (int i = 0; i < vertices_.size(); i++)
	{
		vertMap_[i] = vertices_[i];//���W���R�s�[
	}
#pragma endregion
}

void Drawer::InitializeTexture(TextureData* textureData, const wchar_t* szFile)
{
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
}

void Drawer::TransferTextureBuffer(TextureData* textureData, ID3D12Device* device)
{
	HRESULT result;

	//�q�[�v�ݒ�
	textureData->textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureData->textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureData->textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//���\�[�X�ݒ�
	textureData->textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureData->textureResourceDesc.Format = textureData->metadata.format;
	textureData->textureResourceDesc.Width = textureData->metadata.width; //��
	textureData->textureResourceDesc.Height = (UINT)textureData->metadata.height; //����
	textureData->textureResourceDesc.DepthOrArraySize = (UINT16)textureData->metadata.arraySize;
	textureData->textureResourceDesc.MipLevels = (UINT16)textureData->metadata.mipLevels;
	textureData->textureResourceDesc.SampleDesc.Count = 1;

	////�e�N�X�`���o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&textureData->textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureData->textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureData->texBuff));

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < textureData->metadata.mipLevels; i++)
	{
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