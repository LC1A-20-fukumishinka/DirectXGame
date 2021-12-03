#include "DebugText.h"
#include "TextureMgr.h"
DebugText::DebugText()
{
	spriteIndex = 0;
	debugTex = TextureMgr::Instance()->SpriteLoadTexture(L"Resources/debugfont.png");

	//全てのスプライトデータについて
	for (int i = 0; i < _countof(sprites); i++)
	{
		//スプライトを生成する
		sprites[i].Init(debugTex, { 0,0 });
	}
}

void DebugText::Initialize(ID3D12Device *dev)
{

}
void DebugText::Print( const std::string &text, float x, float y, float scale)
{
	//全ての文字について
	for (int i = 0; i < text.size(); i++)
	{
		//最大文字数超過
		if (spriteIndex >= maxCharCount)
		{
			break;
		}

		//一文字取り出す(※ASCIIコードでしたか成り立たない)
		const unsigned char &character = text[i];

		//ASCIIコードの二段分飛ばした番号を計算
		int fontIndex = character - 32;
		if (character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex].position = { x + fontWidth * scale * i, y, 0 };
		sprites[spriteIndex].texLeftTop = { (float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight };
		sprites[spriteIndex].texSize = { fontWidth, fontHeight };
		sprites[spriteIndex].size = { fontWidth * scale, fontHeight * scale };
		//頂点バッファ転送
		sprites[spriteIndex].SpriteTransferVertexBuffer();
		//更新
		sprites[spriteIndex].SpriteUpdate();
		//文字を一つ進める
		spriteIndex++;
	}

}
void DebugText::DrawAll()
{
	//全ての文字のスプライトについて
	for (int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		sprites[i].SpriteDraw();
	}

	spriteIndex = 0;
}

