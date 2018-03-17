#include "IA.h"


texture2D DiffuseTex : register(CONCAT(t, DIFFUSETEX_SLOT), space1);
sampler colorSampler : register(s0);


#define LIGHTDIR float4(1.0f, -1.0f, -1.0f, 0.0f)
#define LIGHTDIFFCOL float4(.7f, .7f, .7f, 1.0f)

struct Input
{
	float4 pos : SV_POSITION;
	float4 norm : NORMAL;
	float2 texCoord : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	float4 texColor = DiffuseTex.Sample(colorSampler, input.texCoord);
	float4 color = dot(input.norm, LIGHTDIR) * texColor * LIGHTDIFFCOL;
	return color;
}

