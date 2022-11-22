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
	void Initialize(DirectXBasis* dXBas);
	void InitializeTexture(const wchar_t* szFile);
	void TransferTextureBuffer();
	//void SetInitialize( ObjectBasis* model, ID3D12Device* device, int objectNum);
	void Update();

	void Draw( UINT numIndices);

	void copyInit();
	void copyUp();
	void copyDraw();

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

private://�����o�ϐ�
	DirectXBasis* dXBas_ = nullptr;

	Vertex* vertMap_ = nullptr;
	unsigned short* indexMap_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};

	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

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