cbuffer Constants
{
    float4x4 Projection;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color: COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color: COLOR;
};


PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position =  mul(float4(input.Position.xyz, 1.0f), Projection);
    output.Color = input.Color;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_Target
{
    return input.Color;
}