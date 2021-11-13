#include "Object3D.h"


using namespace DirectX;

Object3D::Object3D()
{
	texNumber = 0;
	scale = { 1, 1 , 1 };
	rotation = { 0.0f ,0.0f ,0.0f };
	position = { 0.0f ,0.0f ,0.0f };
	parent = nullptr;

	isInvisible = false;
	color = { 1, 1, 1, 1 };
	matWorld = XMMatrixIdentity();
	this->type = Object3D::Corn;
	
}

void Object3D::Init(const Object3DCommon &object3DCommon, const Camera &camera, UINT texNumber, Object3D *parent)
{
	this->texNumber = texNumber;
	this->parent = parent;

	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();


	//定数バッファの生成
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	//ワールド行列を設定する
	matWorld = GetMatWorld();

	ConstBufferData *constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//色指定(RGBA)
	constMap->mat = matWorld * camera.matView * object3DCommon.matProjection;	//平行透視投影
	constBuff->Unmap(0, nullptr);

}

XMMATRIX Object3D::GetMatWorld()
{
	XMMATRIX matScale, matRot, matTrans, matTmp;


	//各種アフィン変換を行列の形にする
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//各種変換行列を乗算してゆく
	matTmp = XMMatrixIdentity();
	matTmp *= matScale;
	matTmp *= matRot;
	matTmp *= matTrans;

	//もし親オブジェクトがあるなら
	if (parent != nullptr)
	{
		//親オブジェクトのワールド行列を乗算する
		matTmp *= parent->GetMatWorld();
	}

	//生成された行列を返す
	return matTmp;
}

void Object3D::Update(const Object3DCommon &object3DCommon, const Camera &camera)
{
	//ワールド行列を設定する
	matWorld = GetMatWorld();

	ConstBufferData *constMap = nullptr;

	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//色指定(RGBA)
	constMap->mat = matWorld * camera.matView * object3DCommon.matProjection;	//平行透視投影
	constBuff->Unmap(0, nullptr);

}

void Object3D::SetConstBuffer(const Object3DCommon &object3DCommon, const Camera &camera)
{
	ConstBufferData *constMap = nullptr;

	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matWorld = matScale * matWorld;

	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//色指定(RGBA)
	constMap->mat = matWorld * camera.matView * object3DCommon.matProjection;	//平行透視投影
	constBuff->Unmap(0, nullptr);

}

void Object3D::Draw(const Object3DCommon &object3DCommon)
{
	MyDirectX *myD = MyDirectX::GetInstance();


	myD->GetCommandList()->SetPipelineState(object3DCommon.pipelineSet.pipelineState.Get());
	myD->GetCommandList()->SetGraphicsRootSignature(object3DCommon.pipelineSet.rootSignature.Get());

	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//デスクリプタヒープの配列
	ID3D12DescriptorHeap *ppHeaps[] = { object3DCommon.descHeap.Get() };
	myD->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//定数バッファビュー
	myD->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	myD->GetCommandList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			object3DCommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	switch (type)
	{
	case Object3D::Corn:
		//頂点バッファの設定
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.cornVBView);
		//インデックスバッファの設定
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.cornIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.CornNumIndices, 1, 0, 0, 0);
		break;
	case Object3D::Box:
		//頂点バッファの設定
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.boxVBView);
		//インデックスバッファの設定
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.boxIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.BoxNumIndices, 1, 0, 0, 0);
		break;
	case Object3D::Plane:
		//頂点バッファの設定
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.planeVBView);
		//インデックスバッファの設定
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.planeIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.PlaneNumIndices, 1, 0, 0, 0);
		break;
	default://どれにも該当しなかった場合(コーンで描画)
		//頂点バッファの設定
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.cornVBView);
		//インデックスバッファの設定
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.cornIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.CornNumIndices, 1, 0, 0, 0);
		break;
	}

}

void Object3D::SetParent(Object3D *parent)
{
	if (parent == nullptr) return;
	this->parent = parent;
}
