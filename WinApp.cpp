#include "WinApp.h"

void WinApp::Initialize()
{
	//�T�C�Y
	const int window_width = 1280;
	const int window_height = 720;

	const float PI = 3.1415926535f;



	//�N���X�ݒ�
	WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = L"DirectXGame";
	w.hInstance = GetModuleHandle(nullptr);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);

	//OS�ɓo�^
	RegisterClassEx(&w);
	//�T�C�Y
	RECT wrc = { 0,0,window_width,window_height };
	//�����ŃT�C�Y�C��
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindow(w.lpszClassName,
		L"LE2B_01_�A�J�C�P_�J�P��_CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr);

	//�\����Ԃɂ���
	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};//���b�Z�[�W
}

void WinApp::Update()
{
}
