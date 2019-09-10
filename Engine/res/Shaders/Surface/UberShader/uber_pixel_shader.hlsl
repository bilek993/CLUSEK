#include "../../Includes/gamma_correction_utils.hlsli"
#include "../../Includes/normal_utils.hlsli"

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
Texture2D MetalicSmoothnessTexture : TEXTURE : register(t2);
Texture2D OcclusionTexture : TEXTURE : register(t3);
SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 samplerColor = AlbedoTexture.Sample(Sampler, input.TextureCoord).rgb;
    samplerColor = gammaCorrectTexture(samplerColor);

    float3 normalMap = NormalTexture.Sample(Sampler, input.TextureCoord).rgb;

    float3 ambientLight = AmbientLightColor * AmbientLightStrength;
    float3 directionalLight = saturate(dot(DirectionalLightDirection, calculateNormal(normalMap, input.TBN)) * (DirectionalLightColor * DirectionalLightStrength) * samplerColor);

    float3 finalColor = ambientLight * samplerColor;
    finalColor += directionalLight;

    return float4(finalColor, Alpha);
}