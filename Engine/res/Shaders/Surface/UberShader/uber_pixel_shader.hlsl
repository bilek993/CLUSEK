#include "../../Includes/gamma_correction_utils.hlsli"
#include "../../Includes/normal_utils.hlsli"
#include "../../Includes/pbr.hlsli"

cbuffer LightAndAlphaBuffer : register(b0)
{
    float3 DirectionalLightColor;
    float DirectionalLightStrength;
    float3 DirectionalLightDirection;
    float Alpha;
}

cbuffer CameraBuffer : register(b1)
{
    float3 CameraPosition;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3x3 TBN : TBN;
};

Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetalicSmoothnessTexture : register(t2);
Texture2D OcclusionTexture : register(t3);
SamplerState DefaultSampler : register(s0);
SamplerState BrdfSampler : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 albedoColor = AlbedoTexture.Sample(DefaultSampler, input.TextureCoord).rgb;
    float3 normalColor = NormalTexture.Sample(DefaultSampler, input.TextureCoord).rgb;
    float4 metalicSmoothnessColor = MetalicSmoothnessTexture.Sample(DefaultSampler, input.TextureCoord);
    float occlusionColor = MetalicSmoothnessTexture.Sample(DefaultSampler, input.TextureCoord).r;

    float3 albedoColorCorrected = gammaCorrectTexture(albedoColor);
    float3 calculatedNormal = calculateNormal(normalColor, input.TBN);
    float roughness = 1 - metalicSmoothnessColor.a;
    float3 lightColor = DirectionalLightColor * DirectionalLightStrength;

    float3 finalColor = pbr(albedoColorCorrected, calculatedNormal, metalicSmoothnessColor.r, roughness, occlusionColor, 
                            DirectionalLightDirection, lightColor, CameraPosition, input.WorldPosition);

    return float4(finalColor, Alpha);
}