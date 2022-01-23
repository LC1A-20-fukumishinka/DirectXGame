#include "Camera.h"
#include "MyDirectX.h"

using namespace DirectX;

using namespace Projection;
Camera::Camera()
{
	this->eye = { 0.0f, 0.0f, -100.0f };
	this->target = { 0.0f, 0.0f, 0.0f };
	this->up = { 0.0f, 1.0f, 0.0f };
	this->shift = { 0.0f, 0.0f, 0.0f };
	position = { 0.0f, 0.0f, 0.0f };
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matBillBoard = XMMatrixIdentity();
	matBillBoardY = XMMatrixIdentity();

	MyDirectX *myD = MyDirectX::GetInstance();


	projectionData.height = myD->winHeight;
	projectionData.width = myD->winWidth;

	//視錐台行列の生成
	matProjection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(projectionData.angle),				//上下画角60度
		(float)projectionData.width / (float)projectionData.height,	//アスペクト比(画面横幅/画面縦幅)
		projectionData.screenNear, projectionData.screenFar							//前端、奥端
	);
}

void Camera::Init(const DirectX::XMFLOAT3 &eye, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &up, Projection::ProjectionData &projectionData)
{
	this->eye = eye;
	this->target = target;
	this->up = up;
	this->position = position;
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	matView *= XMMatrixTranslation(position.x, position.y, position.z);

	bool isDefault = (projectionData.height <= 0.0f || projectionData.width <= 0.0f);
	if (!isDefault)
	{
		this->projectionData = projectionData;
	}
	//視錐台行列の生成
	matProjection = DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(this->projectionData.angle),				//上下画角60度
		(float)this->projectionData.width / (float)this->projectionData.height,	//アスペクト比(画面横幅/画面縦幅)
		this->projectionData.screenNear, this->projectionData.screenFar							//前端、奥端
	);
}

void Camera::Update()
{
	//matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	//matView *= XMMatrixTranslation(position.x, position.y, position.z);
	MakeMatCamera();
	SetShift(XMFLOAT3(0, 0, 0));
}

void Camera::SetShift(XMFLOAT3 shift)
{
	this->shift = shift;
}

DirectX::XMMATRIX Camera::GetMatBillboard() const
{
	return matBillBoard;
}

DirectX::XMMATRIX Camera::GetMatBillboardY() const
{
	return matBillBoardY;
}

void Camera::MakeMatCamera()
{
	XMFLOAT3 tmpEye =eye + position + shift;
	XMVECTOR eyePosition = XMLoadFloat3(&tmpEye);
	//注視点座標

	XMFLOAT3 tmpTarget = target + position + shift;
	XMVECTOR targetPosition = XMLoadFloat3(&tmpTarget);
	//(仮の) 上方向
	XMVECTOR upVector = XMLoadFloat3(&up);

	//カメラZ軸
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	//0ベクトルだと向が定まらないので除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	//ベクトルの正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);

	//カメラのX軸
	XMVECTOR cameraAxisX;
	//X軸は上方向→Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);

	//カメラのY軸
	XMVECTOR cameraAxisY;
	//Y軸はZ軸→X軸の外積で求まる
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//ベクトルの正規化
	cameraAxisY = XMVector3Normalize(cameraAxisY);

	//カメラ回転行列
	XMMATRIX matCameraRot;
	//カメラ座標系→ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	//転置により逆行列(逆回転)を計算
	matView = XMMatrixTranspose(matCameraRot);

	//視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);
	//一つのベクトルに街メル
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//ビュー行列に平行移動成分を設定
	matView.r[3] = translation;

#pragma region 全方向ビルボード
	//ビルボード
	matBillBoard.r[0] = cameraAxisX;
	matBillBoard.r[1] = cameraAxisY;
	matBillBoard.r[2] = cameraAxisZ;
	matBillBoard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
#pragma endregion


#pragma region Y軸回りビルボード行列計算
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

	//X軸は共通
	ybillCameraAxisX = cameraAxisX;
	//Y軸は和0ルド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z軸はX軸→Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(ybillCameraAxisX, ybillCameraAxisY);

	//Y軸周りビルボード行列
	matBillBoardY.r[0] = ybillCameraAxisX;
	matBillBoardY.r[1] = ybillCameraAxisY;
	matBillBoardY.r[2] = ybillCameraAxisZ;
	matBillBoardY.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
#pragma endregion

}
