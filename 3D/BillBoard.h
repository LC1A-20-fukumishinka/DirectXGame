#pragma once
#include "Object3D.h"
class BillBoard
{
public:
	Object3D obj;
	BillBoard();
	void Init(const Object3DCommon &object3DCommon, const Camera &camera, UINT texNumber, Object3D *parent = nullptr);
	void Update(const Object3DCommon &object3DCommon, const Camera &camera);
	void Draw(const Object3DCommon &object3DCommon);
private:
};