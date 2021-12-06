#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex :register(t0);	//0番スロットに設定されたテクスチャー
SamplerState smp : register(s0);		//0番スロットに設定されたテクスチャー

float4 main(VSOutput input) : SV_TARGET
{
	return tex.Sample(smp, input.uv) * color;
}