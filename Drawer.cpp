#include "Drawer.h"
#include <cassert>
#include <d3dcompiler.h>
#include <string>

void Drawer::Initialize(const wchar_t* vsFile,const wchar_t* psFile){
	
	LoadShaderFile(vsFile,psFile);
	
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

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	SetingGraphicsPipeline();
}

//�O���t�B�b�N�X�p�C�v���C���ݒ�
void Drawer::SetingGraphicsPipeline(){
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