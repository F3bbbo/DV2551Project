#include "IA.h"
//VertexBuffer

StructuredBuffer<float4> pos : register(CONCAT(t, POS), space0);
StructuredBuffer<float4> norm : register(CONCAT(t, NORM), space0);
StructuredBuffer<float2> texCoord : register(CONCAT(t, UVCOORD), space0);

struct Output 
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};


Output main(uint vertexID : SV_VertexID)
{
	Output output;
	output.pos = pos[vertexID];
	output.norm = norm[vertexID];
	output.texCoord = texCoord[vertexID];
	return output;
}

////#include "Defines.h"
//struct Input {
//	float4 pos : POSITION;
//#ifdef NORMALDEF
//	float4 normal : NORMAL;
//#endif
//#ifdef TEXTCOORDDEF
//	float2 texCoord	  : TEXCOORD;
//#endif
//};
//
//struct Output {
//	float4 pos : SV_POSITION;
//#ifdef NORMALDEF
//	float4 normal : NORMAL;
//#endif
//#ifdef TEXTCOORDDEF
//	float2 texCoord : TEXCOORD;
//#endif
//};
//
//cbuffer TRANSLATION_NAMEDEF : register(TRANSLATIONDEF)
//{
//	float4 translate;
//}
//
//cbuffer DIFFUSE_TINT_NAMEDEF : register(DIFFUSE_TINTDEF)
//{
//	float diffuseTint;
//}
//
//
//Output main( Input input )
//{
//	Output output;
//	output.pos = input.pos + translate;
//	output.pos.z = -output.pos.z; // Changing to left oriented space
//
//#ifdef NORMALDEF
//	output.normal = input.normal;
//#endif
//
//#ifdef TEXTCOORDDEF
//	output.texCoord = input.texCoord;
//#endif
//
//	return output;
//}
