#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

class DirectXBasis{

public: //namespaceの省略
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://基本的なメンバ関数

	//初期化
	void Initialize(WinApp* winApp);

public://固有のメンバ関数

	//デバイス初期化
	void InitDevice();

	//コマンド初期化
	void InitCommand();

	//スワップチェーン初期化
	void InitSwapChain();

private://よく使うメンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	///デバイス関連
	//DirectX12のデバイス
	ComPtr<ID3D12Device> device_ = nullptr;
	//DXGIファクトリ
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	///コマンド関連
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	///スワップチェーン関連
	//スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
};