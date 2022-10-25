#include "FixFPS.h"

void FixFPS::Initialize(){
	//現在時間を記録
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update(){
#pragma region 定数の定義
	// 1/60秒ピッタリの時間
	const std::chrono::microseconds
		kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds
		kMinCheckTime(uint64_t(1000000.0f / 65.0f));
#pragma endregion
}