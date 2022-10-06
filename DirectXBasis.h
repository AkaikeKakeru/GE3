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

private://�悭�g�������o�ϐ�
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	///�f�o�C�X�֘A
	//DirectX12�̃f�o�C�X
	ComPtr<ID3D12Device> device = nullptr;
	//DXGI�t�@�N�g��
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;

	///�R�}���h�֘A
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
};