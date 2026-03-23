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
	static Vector2 reflect(const Vector2& v, const Vector2& n);
	Vector2 normalized() const; //Returns a normalized vector
	void Normalize(); //Normalizes the vector 
	float length() const; //Returns length of vector
	
	static float Dot(const Vector2& a, const Vector2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	void operator= (const Vector2 other); //No return type disallows chaining

	Vector2 operator+ (const Vector2 other) const;
	Vector2 operator+ (const float scaler) const;
	void operator+= (const Vector2 other);
	void operator+= (const float scaler);

	Vector2 operator- (const Vector2 other) const;
	Vector2 operator- (const float scaler) const;
	Vector2 operator- () const;
	void operator-= (const Vector2 other);
	void operator-= (const float scaler);

	Vector2 operator* (const Vector2 other) const;
	Vector2 operator* (const float scaler) const;
	void operator*= (const Vector2 other);
	void operator*= (const float scaler);

	Vector2 operator/ (const Vector2 other) const;
	Vector2 operator/ (const float scaler) const;
	void operator/= (const Vector2 other);
	void operator/= (const float scaler);
};

Vector2 operator* (const float scalar, const Vector2& v);