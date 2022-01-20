#pragma once
#include "Wall.h"
#include <vector>
#include "CollisionPrimitive.h"
class WallMgr
{
private:
	WallMgr();
	~WallMgr();
public:
	WallMgr &operator =(const WallMgr &Obj) = delete;
	WallMgr(const WallMgr &obj) = delete;
	static WallMgr *Instance();
	void Init(std::vector<Wall> walls);

	std::vector<Triangle> GetFaces();
private:
	std::vector<Wall> walls;

};