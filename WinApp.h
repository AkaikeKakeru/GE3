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
	//�I��
	void Finalize();

//Getter
public:
	HINSTANCE GetHInstance() const { return w_.hInstance; }
	HWND GetHwnd() const {return hwnd_;}
	MSG GetMsg() const { return msg_; }

//�O���Ŏg����萔
public:
	//�T�C�Y
	static const int WinWidth = 1280;
	static const int WinHeight = 720;

//�悭�g�������o�ϐ�
private:
	//�N���X�ݒ�
	WNDCLASSEX w_{};
	//�E�B���h�E�n���h��
	HWND hwnd_ = nullptr;
	//���b�Z�[�W
	MSG msg_{};
};