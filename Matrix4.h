#pragma once

#include "Vector3.h"

//Matrix4 �\����
struct Matrix4{
	float m[4][4];
};

// �P�ʍs������߂�
Matrix4 MatIdentity();

//�g��k���s��̐ݒ�
Matrix4 MatScale(const Vector3& s);

// ��]�s��̐ݒ�
Matrix4 MatRotationX(float angle);
Matrix4 MatRotationY(float angle);
Matrix4 MatRotationZ(float angle);

//���s�ړ��s��̐ݒ�
Matrix4 MatTranslation(const Vector3& t);

// ���W�ϊ� (�x�N�g���ƍs��̊|���Z������)
Vector3 Vec3Transform(const Vector3& v, const Matrix4& m);

// ������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// 2�����Z�q�I�[�o�[���[�h
const Matrix4 operator* (const Matrix4& m1, const Matrix4& m2);
const Vector3 operator* (const Vector3& v, const Matrix4& m);