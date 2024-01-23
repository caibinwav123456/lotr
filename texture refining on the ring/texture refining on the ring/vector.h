#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <math.h>

class Vector
{
public:
	float x,y;
public:
	Vector(float _x,float _y)
	{
		x=_x,y=_y;
	}
	Vector operator+(Vector& v)
	{
		return Vector(x+v.x,y+v.y);
	}
	Vector operator-(Vector& v)
	{
		return Vector(x-v.x,y-v.y);
	}
	float operator*(Vector& v)
	{
		return x*v.x+y*v.y;
	}
	Vector operator*(float f)
	{
		return Vector(x*f,y*f);
	}

};
float length(Vector v);
Vector norm(Vector v);

#endif