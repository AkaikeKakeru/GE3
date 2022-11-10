#include "Vector3.h"
#include <cmath>	//sqrt

//コンストラクタ
Vector3::Vector3()
	: x(0),y(0),z(0){

}

Vector3::Vector3(float x, float y, float z)
	: x(x),y(y),z(z){

}

//ノルム
float Vector3::Length() const{
	return sqrt((x * x) + (y * y) + (z * z));
}

//正規化(単位化とも言ったっけ?)
Vector3& Vector3::Normalize(){
	float len = Length();
	if(len != 0)
	{
		return *this /= len;
	}
	return *this;
}

//内積
float Vector3::Dot(const Vector3& v) const{
	return this->x * v.x + this->y * v.y + this->z * v.z; 
}

//外積
Vector3 Vector3::Cross(const Vector3& v) const{
	return Vector3(this->y * v.z - this->z * v.y,
		this->z * v.x - this->x * v.z,
		this->x * v.y - this->y * v.x);
}

//単項演算子オーバーロード
Vector3 Vector3::operator+() const{
	return *this;
}

Vector3 Vector3::operator-() const{
	return Vector3(-x,-y,-z);
}

//代入演算子オーバーロード
Vector3& Vector3::operator+=(const Vector3& v) {
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v){
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s){
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator/=(float s){
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const float Vec3Length(Vector3 v){
	return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

const Vector3& Vec3Normalize(Vector3 v){
	float len = Vec3Length(v);
	if(len != 0){
		return v /= len;
	}
	return v;
}

const float Vec3Dot(const Vector3& v1,const Vector3& v2){
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; 
}

const Vector3 Vec3Cross(const Vector3& v1,const Vector3& v2){	
	return Vector3(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

//Vector3　クラスに属さない関数群
//2項演算子
const Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2){
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v,float s){
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v){
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator/(const Vector3& v,float s){
	Vector3 temp(v);
	return temp /= s;
}

//線形補間
const Vector3 lerp(const Vector3& start, const Vector3& end,const float t){
	// 線形
	return start * (1.0f - t) + end * t;
}

const Vector3 EaseIn(const Vector3& start, const Vector3& end, const float t){
	//ease-in
	float y = t*t;
	return start * (1.0f - y) + end * y;
}

const Vector3 EaseOut(const Vector3& start, const Vector3& end, const float t){
	//ease-out
	float y = t * (2 - t);
	return start * (1.0f - y) + end * y;
}

const Vector3 EaseInOut(const Vector3& start, const Vector3& end, const float t){
	//ease-in,ease-out
	float a = t * t;
	float b = 3 - (2 * t);

	float y = a * b;
	return start * (1.0f - y) + end * y;
}

const Vector3 CatmullRomSpline(Vector3 P0,Vector3 P1,Vector3 P2,Vector3 P3,float t){
	Vector3 a = (2 * P1) + ((-P0 + P2) * t);

	Vector3 b = ((2 * P0) - (5 * P1) + (4 * P2) - P3) * t * t;

	Vector3 c = (-P0 + (3 * P1) - (3 * P2) + P3) * t * t * t;

	return 0.5f * (a + b + c);
}