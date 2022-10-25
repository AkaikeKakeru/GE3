#include "FixFPS.h"

void FixFPS::Initialize(){
	//Œ»İŠÔ‚ğ‹L˜^
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update(){
#pragma region ’è”‚Ì’è‹`
	// 1/60•bƒsƒbƒ^ƒŠ‚ÌŠÔ
	const std::chrono::microseconds
		kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60•b‚æ‚è‚í‚¸‚©‚É’Z‚¢ŠÔ
	const std::chrono::microseconds
		kMinCheckTime(uint64_t(1000000.0f / 65.0f));
#pragma endregion
}