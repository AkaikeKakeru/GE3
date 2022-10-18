#include "WinApp.h"
#include "Input.h"
#include "DirectXBasis.h"
#include "Drawer.h"
#include "Model.h"
#include "ViewProjection.h"

#include <dxgi1_6.h>
#include <d3dcompiler.h>//シェーダ用コンパイラ

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")//シェーダ用コンパイラ

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//------WindowsAPI初期化処理 ここから------
	const float PI = 3.1415926535f;

	//ポインタ
	WinApp* winApp = nullptr;
	//WinApp初期化
	winApp = new WinApp();
	winApp->Initialize();

	//------WindowsAPI初期化処理 ここまで------

	//------DirectX初期化処理 ここから------
	//ポインタ
	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	//ポインタ
	DirectXBasis* dXBas = nullptr;
	//DirectXBasis初期化
	dXBas = new DirectXBasis();
	dXBas->Initialize(winApp);
	//------DirectX初期化処理 ここまで------

	//------描画初期化処理 ここから------
#pragma region
	float angle = 0.0f; //カメラの回転角

	//Drawerには今、この辺りの処理を引っ越させてます
	//ポインタ
	Drawer* drawer = nullptr;
	//Drawer初期化
	drawer = new Drawer();
	drawer->Initialize(dXBas,
		L"BasicVS.hlsl", L"BasicPS.hlsl");

#pragma region constMapTransfrom関連

	//ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(Model::ConstBufferDataTransform_) + 0xff) & ~0xff; //256バイトアラインメント
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

#pragma region 三次元オブジェクトの構造化

	//3Dオブジェクトの数
	const size_t kObjectCount = 1;
	//3Dオブジェクトの配列
	//Object3d object3ds[kObjectCount];

	//ポインタ
	Model* modelAdam = nullptr;
	//Model初期化
	modelAdam = new Model();
	modelAdam->Initialize(
		dXBas->GetDevice(),
		input, kObjectCount);

	//配列内の全オブジェクトに対して
	for (int i = 0; i < /*_countof(*/kObjectCount/*)*/; i++)
	{
		//SetIntializeObject3ds(&object3ds[i],dXBas->GetDevice(), i);

		////初期化
		//InitializeObject3d(&object3ds[i], dXBas->GetDevice());
	}

#pragma endregion

#pragma region 単位行列で埋めた後
#pragma region 平行投影行列の計算

	////DirectXMathで用意されている関数に置き換え
	//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
	//	0.0f, window_width,//左端、右端
	//	window_height, 0.0f,//下端、上端
	//	0.0f, 1.0f);//前端、奥端
#pragma endregion

	//ポインタ
	ViewProjection* viewProjection_ = nullptr;
	//ビューとプロジェクションの初期化
	viewProjection_ = new ViewProjection;
	viewProjection_->Initialize();


	//------描画初期化処理 ここまで------
	//ゲームループ
	while (true) {
		//入力更新
		input->Update();

#pragma region ターゲットの周りを回るカメラ
		if (input->ifKeyPress(DIK_D) || input->ifKeyPress(DIK_A))
		{
			if (input->ifKeyPress(DIK_D)) { angle += XMConvertToRadians(1.0f); }
			else if (input->ifKeyPress(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

			//angleラジアンだけY軸周りに回転、半径は-100
			viewProjection_->eye_.x = -100 * sinf(angle);
			viewProjection_->eye_.z = -100 * cosf(angle);
			viewProjection_->matView_ = XMMatrixLookAtLH(XMLoadFloat3(&viewProjection_->eye_),
				XMLoadFloat3(&viewProjection_->target_), XMLoadFloat3(&viewProjection_->up_));

			/*object3ds[0]*/
			modelAdam->GetConstMapTransform()->mat = viewProjection_->matView_ * viewProjection_->matProjection_;
		}
#pragma endregion

#pragma region 連続移動

		//for (size_t i = 0; i < _countof(object3ds); i++)
		//{
		////	//UpdateObject3d(&object3ds[i], matView, matProjection);

		//}
		modelAdam->Update(viewProjection_->matView_, viewProjection_->matProjection_);

		//UpdateObjectControll(&object3ds[0], input);

		//描画の準備
		dXBas->PrepareDraw();

		//コマンドリストの処理を一気に実行
		drawer->Update();

		//全オブジェクトについて処理
		//for (int i = 0; i < _countof(object3ds); i++)
		//{
		//	//DrawObject3d(&object3ds[i], dXBas->GetCommandList(),drawer->GetVBView(),drawer->GetIBView(),static_cast<UINT>( drawer->GetIndices().size()));
		//}
		modelAdam->Draw(dXBas->GetCommandList(), drawer->GetVBView(), drawer->GetIBView(), static_cast<UINT>(drawer->GetIndices().size()));

		//描画後処理
		dXBas->PostDraw();

		//windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			//ループを抜ける
			break;
		}
	}

	//WindowsAPI終了処理
	winApp->Finalize();

	//ビュープロジェクションの解放
	delete viewProjection_;
	viewProjection_ = nullptr;

	//モデルの解放
	delete modelAdam;
	modelAdam = nullptr;

	//描画系の解放
	delete drawer;
	drawer = nullptr;

	//基盤の解放
	delete dXBas;
	dXBas = nullptr;

	//入力の解放
	delete input;
	input = nullptr;

	//WinAppの解放
	delete winApp;
	winApp = nullptr;

	return 0;
}