#include "FixFPS.h"

void FixFPS::Initialize(){
	//���ݎ��Ԃ��L�^
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update(){
}