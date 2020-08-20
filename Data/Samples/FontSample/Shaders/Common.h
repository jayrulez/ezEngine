<<<<<<< HEAD
#pragma once

#include <Shaders/Common/GlobalConstants.h>
#include "SampleConstantBuffer.h"
=======
#include <Shaders/Common/GlobalConstants.h>

CONSTANT_BUFFER(ezMaterialConstants, 0)
{
  MAT4(ViewMatrix);
};
>>>>>>> d078b314d (WIP)

#if EZ_ENABLED(PLATFORM_DX11)

struct VS_IN
{
  float3 Position : POSITION;
<<<<<<< HEAD
  float2 TexCoord0 : TEXCOORD0;
=======
>>>>>>> d078b314d (WIP)
};

struct VS_OUT
{
  float4 Position : SV_Position;
<<<<<<< HEAD
  float2 TexCoord0 : TEXCOORD0;
=======
  float2 FragCoord : TEXCOORD0;
>>>>>>> d078b314d (WIP)
};

typedef VS_OUT PS_IN;

#endif



