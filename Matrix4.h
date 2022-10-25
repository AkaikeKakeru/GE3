#pragma once

#include "Vector3.h"

//Matrix4 構造体
struct Matrix4{
	float m[4][4];
};

// 単位行列を求める
Matrix4 MatIdentity();

//拡大縮小行列の設定
Matrix4 MatScale(const Vector3& s);

// 回転行列の設定
Matrix4 MatRotationX(float angle);
Matrix4 MatRotationY(float angle);
Matrix4 MatRotationZ(float angle);

//平行移動行列の設定
Matrix4 MatTranslation(const Vector3& t);

// 座標変換 (ベクトルと行列の掛け算をする)
Vector3 Vec3Transform(const Vector3& v, const Matrix4& m);

// 代入演算子オーバーロード
Matrix4& operator*=(Matrix4& m1, const Matrix4& m2);

// 2項演算子オーバーロード
const Matrix4 operator* (const Matrix4& m1, const Matrix4& m2);
const Vector3 operator* (const Vector3& v, const Matrix4& m);