#pragma once
#include <chrono>

class FixFPS {
public: //��{�I�Ȋ֐�
	void Initialize();
	void Update();

private: //�ŗL�̃����o�֐�
	//�L�^����(FPS�Œ�p)
	std::chrono::steady_clock::time_point reference_;
};