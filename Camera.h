#pragma once
#include <DirectXMath.h>

namespace Projection
{
	struct ProjectionData
	{
		float angle;
		float width;
		float height;
		float screenNear;
		float screenFar;

		ProjectionData()
		{
			angle = 60.0f;
			width = 0.0f;
			height = 0.0f;
			screenNear = 0.1f;
			screenFar = 1000.0f;
		}
	};

}
class Camera
{
public:
	DirectX::XMMATRIX matProjection;

	DirectX::XMMATRIX matView;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 position;


	Camera();
	void Init(const DirectX::XMFLOAT3 &eye = { 0, 0, -100 }, const  DirectX::XMFLOAT3 &target = { 0, 0, 0 },const DirectX::XMFLOAT3 &position = { 0, 0, 0 }, const  DirectX::XMFLOAT3 &up = { 0, 1, 0 }, Projection::ProjectionData &projectionData = Projection::ProjectionData());
	void Update();

	DirectX::XMMATRIX GetMatBillboard() const;
	DirectX::XMMATRIX GetMatBillboardY() const;
private:
	DirectX::XMMATRIX matBillBoard;
	DirectX::XMMATRIX matBillBoardY;
	Projection::ProjectionData projectionData;

	void MakeMatCamera();

};