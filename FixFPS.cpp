#include "FixFPS.h"

void FixFPS::Initialize(){
	//���ݎ��Ԃ��L�^
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update(){
#pragma region �萔�̒�`
	// 1/60�b�s�b�^���̎���
	const std::chrono::microseconds
		kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60�b���킸���ɒZ������
	const std::chrono::microseconds
		kMinCheckTime(uint64_t(1000000.0f / 65.0f));
#pragma endregion
}