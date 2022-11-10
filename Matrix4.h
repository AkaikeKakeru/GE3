#pragma once

#include "Vector3.h"

//Matrix4 �\����
struct Matrix4{
	float m[4][4];
};

// �P�ʍs������߂�
Matrix4 Mat4Identity();

//�g��k���s��̐ݒ�
Matrix4 Mat4Scale(const Vector3& s);

// ��]�s��̐ݒ�
Matrix4 Mat4RotationX(float angle);
Matrix4 Mat4RotationY(float angle);
Matrix4 Mat4RotationZ(float angle);

//���s�ړ��s��̐ݒ�
Matrix4 Mat4Translation(const Vector3& t);

// ���W�ϊ� (�x�N�g���ƍs��̊|���Z������)
Vector3 Vec3Transform(const Vector3& v, const Matrix4& m);

// ������Z�q�I�[�o�[���[�h
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// 2�����Z�q�I�[�o�[���[�h
const Matrix4 operator* (const Matrix4& m1, const Matrix4& m2);
const Vector3 operator* (const Vector3& v, const Matrix4& m);