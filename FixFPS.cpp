#include "FixFPS.h"

void FixFPS::Initialize(){
	//現在時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update(){
}