#include "../../Includes/gamma_correction_utils.hlsli"

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
    return float4(output, 1.0f);
}