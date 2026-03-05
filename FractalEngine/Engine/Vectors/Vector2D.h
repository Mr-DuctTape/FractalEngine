#pragma once

struct Vector2
{
	float x, y;

	Vector2()
	{
		x = 0;
		y = 0;
	}
	Vector2(float posX, float posY)
	{
		x = posX;
		y = posY;
	}

	Vector2 normalized();
	float length();
	float dot();

	void operator= (const Vector2 other);

	Vector2 operator+ (const Vector2 other) const;
	Vector2 operator+ (const float scaler) const;
	Vector2 operator- (const Vector2 other) const;
	Vector2 operator- (const float scaler) const;
	Vector2 operator- () const;
	Vector2 operator* (const Vector2 other) const;
	Vector2 operator* (const float scaler) const;
};