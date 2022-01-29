#include "Sprite.h"
#include "TextureMgr.h"
#include "MyDirectX.h"
Sprite::Sprite()
{
	anchorpoint = { 0.5f, 0.5f };
	isFlipX = false;
	isFlipY = false;
	position = { 0, 0, 0 };
	color = { 1, 1, 1, 1 };
	rotation = 0.0f;

	texNumber = 0;
	size = { 100, 100 };
	texLeftTop = { 0, 0 };
	texSize = { 0 , 0 };
	isInvisible = false;

	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();
	VertexPosUv vertices[] =
	{
		{},//����
		{},//����
		{},//�E��
		{},//�E��
	};
	//���_�o�b�t�@����
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);


	//���_�o�b�t�@�r���[�̍쐬
	vBView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vBView.SizeInBytes = sizeof(vertices);
	vBView.StrideInBytes = sizeof(vertices[0]);
}

void Sprite::Init(UINT texNumber, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	HRESULT result = S_FALSE;
	MyDirectX* myD = MyDirectX::GetInstance();

	SpriteCommon::Instance();
	this->texNumber = texNumber;

	if (!TextureMgr::Instance()->CheckHandle(texNumber))
	{
		assert(0);
		return;
	}

	enum { LB, LT, RB, RT };
	this->anchorpoint = anchorpoint;
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
	position = { 0, 0, 0 };
	color = { 1, 1, 1, 1 };
	rotation = 0.0f;

	////���_�f�[�^



	////�؂���T�C�Y���摜�̃T�C�Y�ɍ��킹�ĕύX
	D3D12_RESOURCE_DESC resDesc = TextureMgr::Instance()->GetTexBuff(texNumber)->GetDesc();
	texLeftTop = { 0, 0 };
	texSize = { (float)resDesc.Width , (float)resDesc.Height };




	SpriteTransferVertexBuffer();


	//�萔�o�b�t�@�̐���
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	ConstBufferData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = DirectX::XMFLOAT4(1, 1, 1, 1);//�F�w��(RGBA)
	constMap->mat = SpriteCommon::Instance()->matProjection;	//���s�������e
	constBuff->Unmap(0, nullptr);


}

//���_�o�b�t�@�n�̐ݒ�
void Sprite::SpriteTransferVertexBuffer()
{

	if (!TextureMgr::Instance()->CheckHandle(texNumber))
	{
		assert(0);
		return;
	}
	HRESULT result = S_FALSE;
	enum { LB, LT, RB, RT };

	//���_�f�[�^
	VertexPosUv vertices[] = {
		{{}, {0.0f, 1.0f}},//����
		{{}, {0.0f, 0.0f}},//����
		{{}, {1.0f, 1.0f}},//�E��
		{{}, {1.0f, 0.0f}},//�E��
	};

	float left = (0.0f - anchorpoint.x);
	float right = (1.0f - anchorpoint.x);
	float top = (0.0f - anchorpoint.y);
	float bottom = (1.0f - anchorpoint.y);

	if (isFlipX)
	{
		left = -left;
		right = -right;
	}

	if (isFlipY)
	{
		top = -top;
		bottom = -bottom;
	}

	//���_�f�[�^
	vertices[LB].pos = { left,bottom,  0.0f };//����
	vertices[LT].pos = { left, top,  0.0f };//����
	vertices[RB].pos = { right,bottom,  0.0f };//�E��
	vertices[RT].pos = { right, top,  0.0f };//�E��


		//�e�N�X�`�����擾
	D3D12_RESOURCE_DESC resDesc = TextureMgr::Instance()->GetTexBuff(texNumber)->GetDesc();


	float tex_left = texLeftTop.x / resDesc.Width;
	float tex_right = (texLeftTop.x + texSize.x) / resDesc.Width;
	float tex_top = texLeftTop.y / resDesc.Height;
	float tex_bottom = (texLeftTop.y + texSize.y) / resDesc.Height;

	vertices[LB].uv = { tex_left, tex_bottom };
	vertices[LT].uv = { tex_left, tex_top };
	vertices[RB].uv = { tex_right, tex_bottom };
	vertices[RT].uv = { tex_right, tex_top };


	//�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);
}


void Sprite::SpriteDraw()
{

	//�`��t���O��true����Ȃ������瑁�����^�[��
	if (isInvisible) return;

	if (!TextureMgr::Instance()->CheckHandle(texNumber))
	{
		assert(0);
		return;
	}

	MyDirectX* myD = MyDirectX::GetInstance();
	//�p�C�v�����X�e�[�g�̐ݒ�
	myD->GetCommandList()->SetPipelineState(SpriteCommon::Instance()->pipelineSet.pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	myD->GetCommandList()->SetGraphicsRootSignature(SpriteCommon::Instance()->pipelineSet.rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { TextureMgr::Instance()->GetDescriptorHeap() };
	myD->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);


	//���_�o�b�t�@���Z�b�g
	myD->GetCommandList()->IASetVertexBuffers(0, 1, &vBView);

	//�萔�o�b�t�@���Z�b�g
	myD->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	myD->GetCommandList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			TextureMgr::Instance()->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	//�`��R�}���h
	myD->GetCommandList()->DrawInstanced(4, 1, 0, 0);

}

//�萔�o�b�t�@�n�̐ݒ�(�F�ƃA�t�B���ϊ�)
void Sprite::SpriteUpdate()
{
	//���[���h�s��̍X�V
	matWorld = DirectX::XMMatrixIdentity();
	matWorld *= DirectX::XMMatrixScaling(size.x, size.y, 1.0f);

	//Z����]
	matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation));
	//���s�ړ�
	matWorld *= DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//�萔�o�b�t�@�̓]��
	ConstBufferData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matWorld * SpriteCommon::Instance()->matProjection;
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

}
