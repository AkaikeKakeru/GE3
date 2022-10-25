#include "Vector2.h"
#include <cmath> //sqrt

Vector2::Vector2()
	: x(0),y(0){
}

Vector2::Vector2(float x, float y)
	: x(x),y(y){
}

float Vector2::Vec2Length() const{
	return sqrt((x * x) + (y * y));
}

Vector2& Vector2::Vec2Normalize() {
	float len = Vec2Length();
	if(len != 0)
	{
		return *this /= len;
	}
	return *this;
}

float Vector2::Vec2Dot(const Vector2& v) const{
	return this->x* v.x + this->y*v.y;
}

float Vector2::Vec2Cross(const Vector2& v) const{
	return this->x* v.y - this->y*v.x;
}

Vector2 Vector2::operator+() const{
	return *this;
}

Vector2 Vector2::operator-() const{
	return Vector2(-x,-y);
}

Vector2& Vector2::operator+=(const Vector2& v){
	this->x += v.x;
	this->y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v){
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s){
	x *= s;
	y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s){
	x /= s;
	y /= s;
	return *this;
}

//Vector2 ƒNƒ‰ƒX‚É‘®‚³‚È‚¢ŠÖ”ŒQ
//“ñ€‰‰Zq
const Vector2 operator+(const Vector2& v1, const Vector2& v2){
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2){
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v,float s){
	Vector2 temp(v);
	return temp * s;
}

const Vector2 operator*(float s, const Vector2& v){
	return v * s;
}

const Vector2 operator/(const Vector2& v,float s){
	Vector2 temp(v);
	return temp / s;
}