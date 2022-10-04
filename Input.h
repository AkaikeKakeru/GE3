#pragma once
#include <windows.h>
#include <wrl.h>
//using namespace Microsoft::WRL;

#define DIRECT_VERSION 0x0800	//DirectInput�̃o�[�W�����w��
#include <dinput.h>

//����
class Input
{
	//namespace�̏ȗ�
public:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

	//��{�I�ȃ����o�֐�
public:
	//������
	void Initialize(HINSTANCE hInstance,
		HWND hwnd);
	//�X�V
	void Update();

	//�悭�g�������o�ϐ�
private:
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

};
