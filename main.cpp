#include "WinApp.h"
#include "Input.h"
#include "DirectXBasis.h"
#include "Drawer.h"
#include "Model.h"

#include <DirectXMath.h>

#include <string>


#include <dxgi1_6.h>
#include <cassert>

#include <d3dcompiler.h>//�V�F�[�_�p�R���p�C��

#include "Struct.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#pragma comment(lib, "d3dcompiler.lib")//�V�F�[�_�p�R���p�C��

using namespace DirectX;
using namespace Microsoft::WRL;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//------WindowsAPI���������� ��������------
	const float PI = 3.1415926535f;

	//�|�C���^
	WinApp* winApp = nullptr;
	//WinApp������
	winApp = new WinApp();
	winApp->Initialize();

	//------WindowsAPI���������� �����܂�------

	//------DirectX���������� ��������------
	//�|�C���^
	Input* input = nullptr;
	//���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	//�|�C���^
	DirectXBasis* dXBas = nullptr;
	//DirectXBasis������
	dXBas = new DirectXBasis();
	dXBas->Initialize(winApp);
	//------DirectX���������� �����܂�------

	//------�`�揉�������� ��������------
#pragma region
	float angle = 0.0f; //�J�����̉�]�p

	//�g�k�{��
	XMFLOAT3 scale;
	//��]�p
	XMFLOAT3 rotation;
	//���W
	XMFLOAT3 position;
	//���W
	//XMFLOAT3 position1;

	//�g�k�{��
	scale = { 1.0f,1.0f,1.0f };
	//��]�p
	rotation = { 00.0f,00.0f,00.0f };
	//���W
	position = { 0.0f,0.0f,0.0f };
	//position1 = { -20.0f,0.0f,0.0f };

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT3 normal;	//�@���x�N�g��
		XMFLOAT2 uv;		//uv���W
	};

	//Drawer�ɂ͍��A���̕ӂ�̏����������z�����Ă܂�
	//�|�C���^
	Drawer* drawer = nullptr;
	//Drawer������
	drawer = new Drawer();
	drawer->Initialize(dXBas,
		L"BasicVS.hlsl", L"BasicPS.hlsl");

	//�|�C���^
	Model* modelAdam = nullptr;


#pragma region constMapTransfrom�֘A

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(Model::ConstBufferDataTransform_) + 0xff) & ~0xff; //256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

#pragma region �O�����I�u�W�F�N�g�̍\����

	//3D�I�u�W�F�N�g�̐�
	const size_t kObjectCount = 1;
	//3D�I�u�W�F�N�g�̔z��
	//Object3d object3ds[kObjectCount];

	XMFLOAT3 rndScale;
	XMFLOAT3 rndRota;
	XMFLOAT3 rndPos;

	//Model������
	modelAdam = new Model();
	modelAdam->Initialize(
		dXBas->GetDevice(),
		input, kObjectCount);

	//�z����̑S�I�u�W�F�N�g�ɑ΂���
	for (int i = 0; i < /*_countof(*/kObjectCount/*)*/; i++)
	{
		//SetIntializeObject3ds(&object3ds[i],dXBas->GetDevice(), i);

		////������
		//InitializeObject3d(&object3ds[i], dXBas->GetDevice());
	}

#pragma endregion

#pragma region �P�ʍs��Ŗ��߂���
#pragma region ���s���e�s��̌v�Z

	////DirectXMath�ŗp�ӂ���Ă���֐��ɒu������
	//constMapTransform0->mat = XMMatrixOrthographicOffCenterLH(
	//	0.0f, window_width,//���[�A�E�[
	//	window_height, 0.0f,//���[�A��[
	//	0.0f, 1.0f);//�O�[�A���[
#pragma endregion

#pragma region �������e�ϊ��s��̌v�Z

	XMMATRIX matProjection =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(45.0f),//�㉺��p45�x
			(float)WinApp::WinWidth / WinApp::WinHeight,//�A�X�y�N�g��(��ʉ���/��ʏc��)
			0.1f, 1000.0f
		);//�O�[�A���[

#pragma region �r���[�s��̍쐬
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);	//���_���W
	XMFLOAT3 target(0, 0, 0);	//�����_���W
	XMFLOAT3 up(0, 1, 0);		//������x�N�g��
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
		XMLoadFloat3(&target), XMLoadFloat3(&up));

#pragma endregion

#pragma endregion

	//------�`�揉�������� �����܂�------
	//�Q�[�����[�v
	while (true) {
		//���͍X�V
		input->Update();

#pragma region �^�[�Q�b�g�̎�������J����
		if (input->ifKeyPress(DIK_D) || input->ifKeyPress(DIK_A))
		{
			if (input->ifKeyPress(DIK_D)) { angle += XMConvertToRadians(1.0f); }
			else if (input->ifKeyPress(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

			//angle���W�A������Y������ɉ�]�A���a��-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye),
				XMLoadFloat3(&target), XMLoadFloat3(&up));

			/*object3ds[0]*/
			modelAdam->GetConstMapTransform()->mat = matView * matProjection;
		}
#pragma endregion

#pragma region �A���ړ�

		//for (size_t i = 0; i < _countof(object3ds); i++)
		//{
		////	//UpdateObject3d(&object3ds[i], matView, matProjection);

		//}
		modelAdam->Update(matView, matProjection);

		//UpdateObjectControll(&object3ds[0], input);


		//�`��̏���
		dXBas->PrepareDraw();


		//�R�}���h���X�g�̏�������C�Ɏ��s
		drawer->Update();

		//�S�I�u�W�F�N�g�ɂ��ď���
		//for (int i = 0; i < _countof(object3ds); i++)
		//{
		//	//DrawObject3d(&object3ds[i], dXBas->GetCommandList(),drawer->GetVBView(),drawer->GetIBView(),static_cast<UINT>( drawer->GetIndices().size()));
		//}
		modelAdam->Draw(dXBas->GetCommandList(), drawer->GetVBView(), drawer->GetIBView(), static_cast<UINT>(drawer->GetIndices().size()));

		//�`��㏈��
		dXBas->PostDraw();

		//windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage()) {
			//���[�v�𔲂���
			break;
		}
	}

	//WindowsAPI�I������
	winApp->Finalize();

	//�`��n�̉��
	delete drawer;
	drawer = nullptr;

	//��Ղ̉��
	delete dXBas;
	dXBas = nullptr;

	//���͂̉��
	delete input;
	input = nullptr;

	//WinApp�̉��
	delete winApp;
	winApp = nullptr;

	return 0;
}