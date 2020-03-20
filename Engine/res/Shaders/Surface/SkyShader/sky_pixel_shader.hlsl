#include "../../Includes/fog.hlsli"

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
    output = CalculateFog(output, 3000.0f);;
    
    return float4(output, 1.0f);
}