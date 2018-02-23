struct Input
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}


////#include "Defines.h"
//struct Input
//{
//	float4 pos : SV_POSITION;
//#ifdef NORMALDEF
//	float4 normal : NORMAL;
//#endif
//#ifdef TEXTCOORDDEF
//	float2  texCoord  : TEXCOORD;
//#endif
//};
//
//cbuffer DIFFUSE_TINT_NAMEDEF : register(DIFFUSE_TINTDEF)
//{
//	float4 diffuseTint;
//}
//
//#ifdef DIFFUSE_SLOTDEF
//Texture2D DiffuseTexture : register(DIFFUSE_SLOTDEF);
//SamplerState DiffuseSampler : register(s0);
//#endif
//
//
//float4 main(Input input) : SV_TARGET
//{
//#ifdef DIFFUSE_SLOTDEF
//	float4 col = DiffuseTexture.Sample(DiffuseSampler, input.texCoord);
//#else
//	float4 col = float4(1.0f, 1.0f, 1.0f, 1.0f);
//#endif
//	col = col * float4(diffuseTint.rgb, 1.0f);
//
//	return col;
//}
