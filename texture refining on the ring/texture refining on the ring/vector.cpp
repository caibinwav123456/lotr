#include "vector.h"

float length(Vector v)
{
	return (float)sqrt(v.x*v.x+v.y*v.y);
}
Vector norm(Vector v)
{
	return v*(1/length(v));
}
