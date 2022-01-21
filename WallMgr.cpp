#include "WallMgr.h"

using namespace std;
WallMgr::WallMgr()
{
}

WallMgr::~WallMgr()
{
}

WallMgr *WallMgr::Instance()
{
	static WallMgr instance;
	return &instance;
}

void WallMgr::Init(std::vector<Wall> &walls)
{
	this->walls = walls;
}

std::vector<Triangle> WallMgr::GetFaces()
{
	vector<Triangle> wallFaces;
	for (int i = 0; i < walls.size(); i++)
	{
		wallFaces.insert(wallFaces.end(),
			walls[i].GetFaces().begin(),
			walls[i].GetFaces().end());
	}
	return wallFaces;
}

std::vector<Wall> &WallMgr::GetWalls()
{
	return walls;
}

std::vector<DirectX::XMFLOAT3> WallMgr::GetWallPos()
{
	vector<DirectX::XMFLOAT3> wallPos;
	for (int i = 0; i < walls.size(); i++)
	{
		wallPos.push_back(walls[i].GetPos());
	}
	return wallPos;
}
