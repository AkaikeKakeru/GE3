#include "FixFPS.h"

void FixFPS::Initialize(){
	//Œ»İŠÔ‚ğ‹L˜^
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update(){
}