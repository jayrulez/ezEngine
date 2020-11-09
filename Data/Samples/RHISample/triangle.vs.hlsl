//#define DXIL 1

#if !defined(DXIL)
#define DXIL 0
#endif

#if DXIL || VULKAN
#define VERTEX_ATTRIBUTE(type, name, index) [[vk::location(index)]] type name : ATTRIBUTE##index
#else
#define VERTEX_ATTRIBUTE(type, name, index) type name : ATTRIBUTE##index
#endif

struct VS_INPUT
{
    VERTEX_ATTRIBUTE(float3, Position, 0);
    VERTEX_ATTRIBUTE(float4, Color, 1);
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color: COLOR0;
};

cbuffer ProjectionMatrixBuffer : register(b0) 
{
    float4x4 ProjectionMatrix; 
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = mul(ProjectionMatrix, float4(input.Position, 1.0f));
    output.Color = input.Color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_Target
{
    return input.Color;
}