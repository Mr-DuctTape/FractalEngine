#include "Vector2D.h"
#include <iostream>

Vector2 Vector2::normalized() const
{
	const float len = Magnitude();
	if (len == 0)
		return Vector2{ 0,0 };
	return Vector2{ x / len, y / len };
}
void Vector2::Normalize()
{
	const float len = Magnitude();
	if (len == 0)
		return;
	x /= len;
	y /= len;
}

float Vector2::Magnitude() const
{
	return sqrt(x * x + y * y);
}

void Vector2::operator=(const Vector2 other)
{
	x = other.x;
	y = other.y;
}

//Additions
Vector2 Vector2::operator+ (const Vector2 other) const
{
	return Vector2{ x + other.x, y + other.y };
}
Vector2 Vector2::operator+ (const float scaler) const
{
	return Vector2{ x + scaler, y + scaler };
}
void Vector2::operator+= (const Vector2 other)
{
	x += other.x;
	y += other.y;
}
void Vector2::operator+= (const float scaler)
{
	x += scaler;
	y += scaler;
}

//Subtractions
Vector2 Vector2::operator- (const Vector2 other) const
{
	return Vector2{ x - other.x, y - other.y };
}
Vector2 Vector2::operator- (const float scaler) const
{
	return Vector2{ x - scaler, y - scaler };
}
Vector2 Vector2::operator- () const
{
	return Vector2{ -x, -y };
}
void Vector2::operator-= (const Vector2 other)
{
	x -= other.x;
	y -= other.y;
}
void Vector2::operator-= (const float scaler)
{
	x -= scaler;
	y -= scaler;
}

//Multiplications
Vector2 Vector2::operator* (const Vector2 other) const
{
	return Vector2{ x * other.x, y * other.y };
}
Vector2 Vector2::operator* (const float scalar) const
{
	return Vector2{ x * scalar, y * scalar };
}
void Vector2::operator*= (const Vector2 other)
{
	x *= other.x;
	y *= other.y;
}
void Vector2::operator*= (const float scaler)
{
	x *= scaler;
	y *= scaler;
}

//Divisions
Vector2 Vector2::operator/ (const Vector2 other) const
{
	return Vector2{ x / other.x, y / other.y };
}
Vector2 Vector2::operator/ (const float scaler) const
{
	return Vector2{ x / scaler, y / scaler };
}
void Vector2::operator/= (const Vector2 other)
{
	x /= other.x;
	y /= other.y;
}
void Vector2::operator/= (const float scaler)
{
	x /= scaler;
	y /= scaler;
}

Vector2 operator* (const float scalar, const Vector2& v)
{
	return { v.x * scalar, v.y * scalar };
}