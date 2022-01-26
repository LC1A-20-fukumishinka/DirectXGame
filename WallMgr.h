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
	void Init(std::vector<Wall> &walls);
	void Update();
	void Draw();
	std::vector<Triangle> GetFaces();

	std::vector<Wall> &GetWalls();

	std::vector<DirectX::XMFLOAT3> GetWallPos();

	bool CheckWallBullet(const DirectX::XMFLOAT3 &Pos, const  DirectX::XMFLOAT3 Speed);
private:
	std::vector<Wall> walls;

};