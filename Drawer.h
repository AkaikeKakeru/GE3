#pragma once
#include <wrl.h>

class  Drawer
{
public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public://��{�I�ȃ����o�֐�
	void Initialize();

private: //�悭�g�������o�ϐ�

};