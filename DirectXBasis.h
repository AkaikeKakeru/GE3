#pragma once
#include "WinApp.h"

class DirectXBasis{
public://��{�I�ȃ����o�֐�
public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

	//������
	void Initialize(WinApp* winApp);

private://�悭�g�������o�ϐ�
	WinApp* winApp_ = nullptr;
};