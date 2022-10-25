#include "DrawBasis.h"
#include <DirectXMath.h>
#include "Vector3.h"
#include <wrl.h>

using namespace DirectX;

void DrawBasis::Initialize(){
	//���_�f�[�^
	Vector3 vertices[] = {
		//x		 y		z
		{-5.0f, -5.0f, -5.0f},//����
		{-5.0f,  5.0f, -5.0f},//����
		{ 5.0f, -5.0f, -5.0f},//�E��
	};

	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(Vector3) * _countof(vertices));
}