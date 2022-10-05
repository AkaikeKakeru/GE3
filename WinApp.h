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

//Getter
public:
	HINSTANCE GetHInstance() const { return w.hInstance; }
	HWND GetHwnd() const {return hwnd;}

//�O���Ŏg����萔
public:
	//�T�C�Y
	static const int window_width = 1280;
	static const int window_height = 720;

//�悭�g�������o�ϐ�
private:
	//�N���X�ݒ�
	WNDCLASSEX w{};
	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;
};