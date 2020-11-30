Texture2D LogoTexture;
SamplerState LogoSampler;

cbuffer UB0 : register(b0)
{
    float4x4 Projection;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    //float4 Color: COLOR;
    float2 UV: TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    //float4 Color: COLOR0;
    float2 UV: TEXCOORD0;
};


PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = mul(Projection, float4(input.Position, 1.0));
    //output.Color = input.Color;
    output.UV = input.UV;
    return output;
}

float4 PSMain(PS_INPUT input) : SV_Target
{
    //return input.Color;
    return LogoTexture.Sample(LogoSampler, input.UV);
}