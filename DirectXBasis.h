#pragma once

class DirectXBasis{
public://��{�I�Ȋ֐�
public: //namespace�̏ȗ�
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

	//������
	void Initialize();
};