#include"BillBoard.h"

using namespace DirectX;
BillBoard::BillBoard()
{
	obj.type = Object3D::Plane;
}

void BillBoard::Init(const Object3DCommon &object3DCommon, const Camera &camera, UINT texNumber, Object3D *parent)
{
	obj.Init(object3DCommon, camera, texNumber);
	obj.type = Object3D::Plane;

		//ワールド行列を設定する
	obj.matWorld = XMMatrixIdentity();

	obj.matWorld *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
	obj.matWorld *= XMMatrixRotationZ(XMConvertToRadians(obj.rotation.z));
	obj.matWorld *= camera.GetMatBillboard();
	obj.matWorld *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

		Object3D::ConstBufferData *constMap = nullptr;
	HRESULT result = obj.constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//色指定(RGBA)
	constMap->mat = obj.matWorld * camera.matView * object3DCommon.matProjection;	//平行透視投影
	obj.constBuff->Unmap(0, nullptr);

}

void BillBoard::Update(const Object3DCommon &object3DCommon, const Camera &camera)
{
	//ワールド行列を設定する
	obj.matWorld = XMMatrixIdentity();

	obj.matWorld *= XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
	obj.matWorld *= XMMatrixRotationZ(XMConvertToRadians(obj.rotation.z));
	obj.matWorld *= camera.GetMatBillboard();
	obj.matWorld *= XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

	Object3D::ConstBufferData *constMap = nullptr;
	HRESULT result = obj.constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//色指定(RGBA)
	constMap->mat = obj.matWorld * camera.matView * object3DCommon.matProjection;	//平行透視投影
	obj.constBuff->Unmap(0, nullptr);
}

void BillBoard::Draw(const Object3DCommon &object3DCommon, PipeClass::PipelineSet pipelineSet)
{
	obj.Draw(object3DCommon, pipelineSet);
}
