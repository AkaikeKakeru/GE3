#pragma once
#include "DirectXBasis.h"

class DrawBasis
{
private:
	template <class Type>
	using ComPtr = Microsoft::WRL::ComPtr<Type>;

public: //��{�I�ȃ����o�֐�
	void Initialize(DirectXBasis* dXBas);

private: //�ŗL�̃����o�֐�
	void LoadInstance(DirectXBasis* dXBas);
	void CreateVertexBufferView();
	void CompileShaderFile();

private:
	DirectXBasis* dXBas_ = nullptr;
};