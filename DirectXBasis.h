#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXBasis{

public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://��{�I�ȃ����o�֐�

	//������
	void Initialize(WinApp* winApp);

public://�ŗL�̃����o�֐�

	//�f�o�C�X������
	void InitDevice();

	//�R�}���h������
	void InitCommand();

	//�X���b�v�`�F�[��������
	void InitSwapChain();

	//�����_�\�^�[�Q�b�g�r���[������
	void InitRTV();

private://�悭�g�������o�ϐ�
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	///�f�o�C�X�֘A
	//DirectX12�̃f�o�C�X
	ComPtr<ID3D12Device> device_ = nullptr;
	//DXGI�t�@�N�g��
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	///�R�}���h�֘A
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	///�X���b�v�`�F�[���֘A
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	///�����_�\�^�[�Q�b�g�r���[�֘A
	//�����_�\�^�[�Q�b�g�r���[�q�[�v
	ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	//�o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>> backBuffers_;
};