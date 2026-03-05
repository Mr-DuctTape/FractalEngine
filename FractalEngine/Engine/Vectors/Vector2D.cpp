#include "Vector2D.h"
#include <iostream>

Vector2 Vector2::normalized()
{
	float len = length();
	return Vector2{ x / len, y / len };
}
float Vector2::dot()
{
	return 0;
}
float Vector2::length()
{
	return sqrt(x * x + y * y);
}

void Vector2::operator=(const Vector2 other)
{
	x = other.x;
	y = other.y;
}

Vector2 Vector2::operator+ (const Vector2 other) const
{
	return Vector2{ x + other.x, y + other.y };
}
Vector2 Vector2::operator+ (const float scaler) const
{
	return Vector2{ x + scaler, y + scaler };
}

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

Vector2 Vector2::operator* (const Vector2 other) const
{
	return Vector2{ x * other.x, y * other.y };
}
Vector2 Vector2::operator* (const float scalar) const
{
	return Vector2{ x * scalar, y * scalar };
}