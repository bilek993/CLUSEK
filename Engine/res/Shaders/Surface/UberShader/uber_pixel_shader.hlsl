#include "../../Includes/gamma_correction_utils.hlsli"

cbuffer LightAndAlphaBuffer : register(b0)
{
    float3 AmbientLightColor;
    float AmbientLightStrength;
    float3 DirectionalLightColor;
    float DirectionalLightStrength;
    float3 DirectionalLightDirection;
    float Alpha;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3x3 TBN : TBN;
};

Texture2D AlbedoTexture : TEXTURE : register(t0);
Texture2D NormalTexture : TEXTURE : register(t1);
SamplerState Sampler : SAMPLER : register(s0);

float3 calculateNormal(PS_INPUT input)
{
    float3 normalMap = NormalTexture.Sample(Sampler, input.TextureCoord).rgb;
    normalMap = (2.0f * normalMap) - 1.0f;
    normalMap.g *= -1.0f;

    return normalize(mul(normalMap, input.TBN));
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 samplerColor = AlbedoTexture.Sample(Sampler, input.TextureCoord).rgb;
    samplerColor = gammaCorrectTexture(samplerColor);

    float3 ambientLight = AmbientLightColor * AmbientLightStrength;
    float3 directionalLight = saturate(dot(DirectionalLightDirection, calculateNormal(input)) * (DirectionalLightColor * DirectionalLightStrength) * samplerColor);

    float3 finalColor = ambientLight * samplerColor;
    finalColor += directionalLight;

    return float4(finalColor, Alpha);
}