#include "../Includes/gamma_correction_utils.hlsli"

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : TEXTURE : register(t0);
SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = InputTexture.Sample(Sampler, input.TextureCoord).rgb;
    output = gammaCorrectFinal(output);
    return float4(output, 1.0f);
}