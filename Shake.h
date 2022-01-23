#pragma once
#include "Vector3.h"
class Shake
{
public:
	static Vector3 GetShake(float power,bool isX = true, bool isY = true, bool isZ = true);
private:
};