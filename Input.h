#pragma once
#include <windows.h>

//����
class Input
{
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
