#include "Object3D.h"
#include "TextureMgr.h"
#include "Model.h"
using namespace DirectX;

Object3D::Object3D()
{
	scale = { 1, 1 , 1 };
	rotation = { 0.0f ,0.0f ,0.0f };
	position = { 0.0f ,0.0f ,0.0f };
	parent = nullptr;

	isInvisible = false;
	color = { 1, 1, 1, 1 };
	matWorld = XMMatrixIdentity();
	this->type = Object3D::Corn;


	//�萔�o�b�t�@�̐���
	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);
	
}

void Object3D::Init( const Camera &camera, Object3D *parent)
{
	this->parent = parent;

	HRESULT result = S_FALSE;




	//���[���h�s���ݒ肷��
	matWorld = GetMatWorld();

	ConstBufferData *constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = color;//�F�w��(RGBA)
	constMap->mat = matWorld * camera.matView * camera.matProjection;	//���s�������e
	constBuff->Unmap(0, nullptr);

}

XMMATRIX Object3D::GetMatWorld()
{
	XMMATRIX matScale, matRot, matTrans, matTmp;


	//�e��A�t�B���ϊ����s��̌`�ɂ���
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);

	//�e��ϊ��s�����Z���Ă䂭
	matTmp = XMMatrixIdentity();
	matTmp *= matScale;
	matTmp *= matRot;
	matTmp *= matTrans;

	//�����e�I�u�W�F�N�g������Ȃ�
	if (parent != nullptr)
	{
		//�e�I�u�W�F�N�g�̃��[���h�s�����Z����
		matTmp *= parent->GetMatWorld();
	}

	//�������ꂽ�s���Ԃ�
	return matTmp;
}

void Object3D::Update(const Camera &camera)
{
	//���[���h�s���ݒ肷��
	matWorld = GetMatWorld();

	ConstBufferData *constMap = nullptr;

	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = color;//�F�w��(RGBA)
	constMap->mat = matWorld * camera.matView * camera.matProjection;	//�������e
	constBuff->Unmap(0, nullptr);

}

void Object3D::SetConstBuffer( const Camera &camera)
{
	ConstBufferData *constMap = nullptr;

	XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);

	matWorld = matScale * matWorld;

	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = color;//�F�w��(RGBA)
	constMap->mat = matWorld * camera.matView * camera.matProjection;	//�������e
	constBuff->Unmap(0, nullptr);

}

void Object3D::Draw(const Object3DCommon &object3DCommon, PipeClass::PipelineSet pipelineSet, int textureNumber)
{
	MyDirectX *myD = MyDirectX::GetInstance();


	myD->GetCommandList()->SetPipelineState(pipelineSet.pipelineState.Get());
	myD->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());

	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�f�X�N���v�^�q�[�v�̔z��

	ID3D12DescriptorHeap *descHeap = TextureMgr::Instance()->GetDescriptorHeap();
	ID3D12DescriptorHeap *ppHeaps[] = { descHeap };
	myD->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�萔�o�b�t�@�r���[
	myD->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());


	if (!TextureMgr::Instance()->CheckHandle(textureNumber))
	{
		assert(0);
		return;
	}
	myD->GetCommandList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeap->GetGPUDescriptorHandleForHeapStart(),
			textureNumber,
			myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);

#pragma region �Ƃ肠������������o�����`��R�}���h
	myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.boxVBView);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.boxIBView);
	myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.BoxNumIndices, 1, 0, 0, 0);
#pragma endregion

	switch (type)
	{
	case Object3D::Corn:
		//���_�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.cornVBView);
		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.cornIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.CornNumIndices, 1, 0, 0, 0);
		break;
	case Object3D::Box:
		//���_�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.boxVBView);
		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.boxIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.BoxNumIndices, 1, 0, 0, 0);
		break;
	case Object3D::Plane:
		//���_�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.planeVBView);
		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.planeIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.PlaneNumIndices, 1, 0, 0, 0);
		break;
	default://�ǂ�ɂ��Y�����Ȃ������ꍇ(�R�[���ŕ`��)
		//���_�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetVertexBuffers(0, 1, &object3DCommon.cornVBView);
		//�C���f�b�N�X�o�b�t�@�̐ݒ�
		myD->GetCommandList()->IASetIndexBuffer(&object3DCommon.cornIBView);
		myD->GetCommandList()->DrawIndexedInstanced(object3DCommon.CornNumIndices, 1, 0, 0, 0);
		break;
	}

}

void Object3D::modelDraw(const ModelObject &model, PipeClass::PipelineSet pipelineSet, bool isSetTexture, int textureNumber)
{
	MyDirectX *myD = MyDirectX::GetInstance();


	myD->GetCommandList()->SetPipelineState(pipelineSet.pipelineState.Get());
	myD->GetCommandList()->SetGraphicsRootSignature(pipelineSet.rootSignature.Get());

	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�f�X�N���v�^�q�[�v�̔z��

	ID3D12DescriptorHeap *descHeap = TextureMgr::Instance()->GetDescriptorHeap();
	ID3D12DescriptorHeap *ppHeaps[] = { descHeap };
	myD->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�萔�o�b�t�@�r���[
	myD->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());

	myD->GetCommandList()->SetGraphicsRootConstantBufferView(1, model.constBuffB1->GetGPUVirtualAddress());

	if(isSetTexture)
	{
		if (!TextureMgr::Instance()->CheckHandle(textureNumber))
		{
			assert(0);
			return;
		}
		myD->GetCommandList()->SetGraphicsRootDescriptorTable(2,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeap->GetGPUDescriptorHandleForHeapStart(),
				textureNumber,
				myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}
	else
	{
		if (!TextureMgr::Instance()->CheckHandle(model.textureHandle))
		{
			assert(0);
			return;
		}
		myD->GetCommandList()->SetGraphicsRootDescriptorTable(2,
			CD3DX12_GPU_DESCRIPTOR_HANDLE(
				descHeap->GetGPUDescriptorHandleForHeapStart(),
				model.textureHandle,
				myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			)
		);
	}

#pragma region �Ƃ肠������������o�����`��R�}���h
	myD->GetCommandList()->IASetVertexBuffers(0, 1, &model.vbView);
	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	myD->GetCommandList()->IASetIndexBuffer(&model.ibView);
	myD->GetCommandList()->DrawIndexedInstanced(model.indices.size(), 1, 0, 0, 0);
#pragma endregion

}

void Object3D::SetParent(Object3D *parent)
{
	if (parent == nullptr) return;
	this->parent = parent;
}

void DepthReset()
{
	MyDirectX *myDirectX = MyDirectX::GetInstance();
	myDirectX->GetCommandList()->ClearDepthStencilView(myDirectX->GetDsvH(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}
