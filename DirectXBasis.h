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

private://よく使うメンバ変数
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	///デバイス関連
	//DirectX12のデバイス
	ComPtr<ID3D12Device> device = nullptr;
	//DXGIファクトリ
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;

	///コマンド関連
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
};