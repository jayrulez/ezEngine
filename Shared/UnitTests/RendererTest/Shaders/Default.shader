[PLATFORMS] 
ALL

[PERMUTATIONS]

[VERTEXSHADER]

#include "Common.inc"

VS_OUT main(VS_IN Input)
{
  VS_OUT RetVal;
  RetVal.pos = mul(mvp, float4(Input.pos, 1.0));

  return RetVal;
}

[PIXELSHADER]

#include "Common.inc"

Texture2D TexDiffuse : register(t0);
SamplerState TexDiffuseSampler : register(s0);

float4 main(PS_IN Input) : SV_Target
{
  return ObjectColor;
  //return TexDiffuse.Sample(TexDiffuseSampler, float2(0.5, 0.5));
}

