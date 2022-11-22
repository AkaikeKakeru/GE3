#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4.h"
#include <d3d12.h>
#include <DirectXTex.h>
#include <wrl.h>

#include "DirectXBasis.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

using namespace DirectX;

class ObjectBasis {
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: // �\����
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

	struct TextureData {
		TexMetadata metadata{};
		ScratchImage scratchImg{};
		ScratchImage mipChine{};

		D3D12_HEAP_PROPERTIES textureHeapProp{};
		D3D12_RESOURCE_DESC textureResourceDesc{};

		//�e�N�X�`���o�b�t�@�̐���
		ComPtr<ID3D12Resource> texBuff = nullptr;
	};

	//���_�f�[�^�\����
	struct Vertex {
		Vector3 pos;		//xyz���W
		Vector3 normal;	//�@���x�N�g��
		Vector2 uv;		//uv���W
	};

public://�����o�֐�
	void Initialize(ComPtr<ID3D12Device> device);
	void InitializeTexture(const wchar_t* szFile);
	void TransferTextureBuffer();
	void Update();

	void Draw( /*UINT numIndices*/);

	void copyInit(DirectXBasis*dXBas);
	void copyUp();
	void copyDraw();

	void PrepareDraw(ComPtr<ID3D12GraphicsCommandList> cmdList);
	void PostDraw();

public:// �萔
	static const int SurfaceNum = 6;
	static const int VertexNum = 4;
	static const int VeticesNum = SurfaceNum * VertexNum;

	static const int IndexNum = SurfaceNum * 2 * 3;
private://static
	//���_�f�[�^
	static std::vector<Vertex> vertices_;
	//�C���f�b�N�X
	static std::vector<unsigned short> indices_;

	//�f�o�C�X
	static ComPtr<ID3D12Device> device_;

	//�R�}���h���X�g
	static ComPtr<ID3D12GraphicsCommandList> cmdList_;

private://�����o�ϐ�
	DirectXBasis* dXBas_ = nullptr;

	D3D12_RESOURCE_DESC resDesc_{};

	Vertex* vertMap_ = nullptr;
	unsigned short* indexMap_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	ComPtr<ID3DBlob> vsBlob_ = nullptr;//���_�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob_ = nullptr;//�s�N�Z���V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob_ = nullptr;//�G���[�I�u�W�F�N�g

	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffMaterial_ = {};
	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataMaterial* constMapMaterial_ = {};

	//�萔�o�b�t�@(�s��p)
	ComPtr<ID3D12Resource> constBuffTransform_ = {};
	//�萔�o�b�t�@�}�b�v(�s��p)
	ConstBufferDataTransform* constMapTransform_ = {};

	ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;

	//�萔�o�b�t�@(�r���[�v���W�F�N�V����)
	ViewProjection* viewPro_ = nullptr;
	//���[���h�ϊ�
	//�A�t�B���ϊ����
	WorldTransform worldTransform_{};

	//�e�I�u�W�F�N�g�ւ̃|�C���^
	ObjectBasis* parent_ = nullptr;

	TextureData textureData_;
};