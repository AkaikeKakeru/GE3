#pragma once
#include <Windows.h>

class WinApp {
//�ÓI�����o�֐�(static)
public:
	static LRESULT WindowProc(
		HWND hwnd,
		UINT msg,
		WPARAM wparam,
		LPARAM lparam);

//��{�I�ȃ����o�֐�
public:
	//������
	void Initialize();
	//�X�V
	void Update();

//�悭�g�������o�ϐ�
private:
	//�T�C�Y
	const int window_width = 1280;
	const int window_height = 720;

	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;
};