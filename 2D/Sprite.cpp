#include "Sprite.h"

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
}

void Sprite::Init(int window_width, int window_height, UINT texNumber, const SpriteCommon &spriteCommon, DirectX::XMFLOAT2 anchorpoint, bool isFlipX, bool isFlipY)
{
	HRESULT result = S_FALSE;
	MyDirectX *myD = MyDirectX::GetInstance();

	enum { LB, LT, RB, RT };
	anchorpoint = anchorpoint;
	this->isFlipX = isFlipX;
	this->isFlipY = isFlipY;
	position = { 0, 0, 0 };
	color = { 1, 1, 1, 1 };
	rotation = 0.0f;

	////���_�f�[�^
	VertexPosUv vertices[] =
	{
		{},//����
		{},//����
		{},//�E��
		{},//�E��
	};

	this->texNumber = texNumber;

	//�؂���T�C�Y���摜�̃T�C�Y�ɍ��킹�ĕύX
	D3D12_RESOURCE_DESC resDesc = spriteCommon.texBuff[texNumber]->GetDesc();
	texLeftTop = { 0, 0 };
	texSize = { (float)resDesc.Width , (float)resDesc.Height };


	//���_�o�b�t�@����
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff)
	);

	SpriteTransferVertexBuffer(spriteCommon);

	//���_�o�b�t�@�r���[�̍쐬
	vBView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vBView.SizeInBytes = sizeof(vertices);
	vBView.StrideInBytes = sizeof(vertices[0]);

	//�萔�o�b�t�@�̐���
	result = myD->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);

	ConstBufferData *constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->color = DirectX::XMFLOAT4(1, 1, 1, 1);//�F�w��(RGBA)
	constMap->mat = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, window_width, window_height, 0.0f, 0.0f, 1.0f);	//���s�������e
	constBuff->Unmap(0, nullptr);


}

//���_�o�b�t�@�n�̐ݒ�
void Sprite::SpriteTransferVertexBuffer(const SpriteCommon &spriteCommon)
{
	HRESULT result = S_FALSE;
	enum { LB, LT, RB, RT };

	//���_�f�[�^
	VertexPosUv vertices[] = {
		{{}, {0.0f, 1.0f}},//����
		{{}, {0.0f, 0.0f}},//����
		{{}, {1.0f, 1.0f}},//�E��
		{{}, {1.0f, 0.0f}},//�E��
	};

	float left = (0.0f - anchorpoint.x) * size.x;
	float right = (1.0f - anchorpoint.x) * size.x;
	float top = (0.0f - anchorpoint.x) * size.y;
	float bottom = (1.0f - anchorpoint.x) * size.y;

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

	//�w��ԍ��̉摜���ǂݍ��ݍς݂Ȃ�
	if (spriteCommon.texBuff[texNumber])
	{
		//�e�N�X�`�����擾
		D3D12_RESOURCE_DESC resDesc =
			spriteCommon.texBuff[texNumber]->GetDesc();

		float tex_left = texLeftTop.x / resDesc.Width;
		float tex_right = (texLeftTop.x + texSize.x) / resDesc.Width;
		float tex_top = texLeftTop.y / resDesc.Height;
		float tex_bottom = (texLeftTop.y + texSize.y) / resDesc.Height;

		vertices[LB].uv = { tex_left, tex_bottom };
		vertices[LT].uv = { tex_left, tex_top };
		vertices[RB].uv = { tex_right, tex_bottom };
		vertices[RT].uv = { tex_right, tex_top };
	}

	//�o�b�t�@�ւ̃f�[�^�]��
	VertexPosUv *vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void **)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);
}

void SpriteCommonBeginDraw(const SpriteCommon &spriteCommon)
{
	MyDirectX *myD = MyDirectX::GetInstance();
	//�p�C�v�����X�e�[�g�̐ݒ�
	myD->GetCommandList()->SetPipelineState(spriteCommon.pipelineSet.pipelineState.Get());
	//���[�g�V�O�l�`���̐ݒ�
	myD->GetCommandList()->SetGraphicsRootSignature(spriteCommon.pipelineSet.rootSignature.Get());
	//�v���~�e�B�u�`���ݒ�
	myD->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap *ppHeaps[] = { spriteCommon.descHeap.Get() };
	myD->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

}

void Sprite::SpriteDraw(const SpriteCommon &spriteCommon)
{

	//�`��t���O��true����Ȃ������瑁�����^�[��
	if (isInvisible) return;
	MyDirectX *myD = MyDirectX::GetInstance();

	//���_�o�b�t�@���Z�b�g
	myD->GetCommandList()->IASetVertexBuffers(0, 1, &vBView);

	//�萔�o�b�t�@���Z�b�g
	myD->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	//�V�F�[�_�[���\�[�X�r���[���Z�b�g
	myD->GetCommandList()->SetGraphicsRootDescriptorTable(1,
		CD3DX12_GPU_DESCRIPTOR_HANDLE(
			spriteCommon.descHeap->GetGPUDescriptorHandleForHeapStart(),
			texNumber,
			myD->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		)
	);
	//�`��R�}���h
	myD->GetCommandList()->DrawInstanced(4, 1, 0, 0);

}

//�萔�o�b�t�@�n�̐ݒ�(�F�ƃA�t�B���ϊ�)
void Sprite::SpriteUpdate(const SpriteCommon &spriteCommon)
{
	//���[���h�s��̍X�V
	matWorld = DirectX::XMMatrixIdentity();
	//Z����]
	matWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation));
	//���s�ړ�
	matWorld *= DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//�萔�o�b�t�@�̓]��
	ConstBufferData *constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void **)&constMap);
	constMap->mat = matWorld * spriteCommon.matProjection;
	constMap->color = color;
	constBuff->Unmap(0, nullptr);

}