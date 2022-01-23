#include "Shake.h"
#include "cmath"
Vector3 Shake::GetShake(float power, bool isX, bool isY, bool isZ)
{

	float FixRate= RAND_MAX / 2;
	Vector3 v(((float)rand() - FixRate) * isX, ((float)rand() - FixRate) * isY, ((float)rand() - FixRate) * isZ);
	v = v.normalaize();
	v *= power;
	return v;
}
