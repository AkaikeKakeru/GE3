#pragma once

class FixFPS {
private: //基本的な関数
	void Initialize();
	void Update();
public: //ゲッター
	void GetInitialize() {
		Initialize();
	}

	void GetUpdate() {
		Update();
	}
};