#pragma once
#include <DirectXMath.h>
class Camera
{
public:

	DirectX::XMMATRIX matView;
	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 target;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 position;


	Camera();
	void Init(const DirectX::XMFLOAT3 &eye = { 0, 0, -100 }, const  DirectX::XMFLOAT3 &target = { 0, 0, 0 },const DirectX::XMFLOAT3 &position = { 0, 0, 0 }, const  DirectX::XMFLOAT3 &up = { 0, 1, 0 });
	void Update();

	DirectX::XMMATRIX GetMatBillboard() const;
	DirectX::XMMATRIX GetMatBillboardY() const;
private:
	DirectX::XMMATRIX matBillBoard;
	DirectX::XMMATRIX matBillBoardY;
	void MakeMatCamera();


};