#pragma once

class FixFPS {
private: //��{�I�Ȋ֐�
	void Initialize();
	void Update();
public: //�Q�b�^�[
	void GetInitialize() {
		Initialize();
	}

	void GetUpdate() {
		Update();
	}
};