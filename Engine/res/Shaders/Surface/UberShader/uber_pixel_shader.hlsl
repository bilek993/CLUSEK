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

Texture2D AlbedoTexture : TEXTURE : register(t0);
Texture2D NormalTexture : TEXTURE : register(t1);
Texture2D MetalicSmoothnessTexture : TEXTURE : register(t2);
Texture2D OcclusionTexture : TEXTURE : register(t3);
SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 albedoColor = AlbedoTexture.Sample(Sampler, input.TextureCoord).rgb;
    float3 normalColor = NormalTexture.Sample(Sampler, input.TextureCoord).rgb;
    float2 metalicSmoothnessColor = MetalicSmoothnessTexture.Sample(Sampler, input.TextureCoord).ra;
    float occlusionColor = MetalicSmoothnessTexture.Sample(Sampler, input.TextureCoord).r;

    float3 albedoColorCorrected = gammaCorrectTexture(albedoColor);
    float3 calculatedNormal = calculateNormal(normalColor, input.TBN);
    float roughness = 1 - metalicSmoothnessColor.y;

    float3 finalColor = pbr(albedoColorCorrected, calculatedNormal, metalicSmoothnessColor.x, roughness, occlusionColor, 
                            DirectionalLightDirection, DirectionalLightColor * DirectionalLightStrength, CameraPosition, 
                            input.WorldPosition);

    return float4(finalColor, Alpha);
}