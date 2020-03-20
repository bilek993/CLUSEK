#include "../../Includes/fog.hlsli"

cbuffer FogBuffer : register(b0)
{
    float3 FogColor;
    float FogDensity;
    float SkyConstantValue;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 TextureCoord : TEXCOORD;
};

TextureCube SkyMap : register(t0);
SamplerState Sampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = SkyMap.Sample(Sampler, input.TextureCoord).rgb;
    output = CalculateFog(output, SkyConstantValue, FogDensity, FogColor);;
    
    return float4(output, 1.0f);
}