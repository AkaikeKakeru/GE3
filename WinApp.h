#pragma once
#include <Windows.h>

class WinApp {
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
};