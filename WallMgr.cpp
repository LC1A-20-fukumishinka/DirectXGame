#include "WallMgr.h"

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

void WallMgr::Init(std::vector<Wall> walls)
{
	this->walls = walls;
}

std::vector<Triangle> WallMgr::GetFaces()
{
	for (int i = 0; i < walls.size(); i++)
	{

	}
	return std::vector<Triangle>();
}
