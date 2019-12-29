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
    float4 LightSpacePosition : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3x3 TBN : TBN;
};

Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetalicSmoothnessTexture : register(t2);
Texture2D OcclusionTexture : register(t3);
Texture2D EmissionTexture : register(t4);
TextureCube IrradianceTexture : register(t5);
TextureCube RadianceTexture : register(t6);
Texture2D BrdfLut : register(t7);
Texture2D ShadowMap : register(t8);
SamplerState DefaultSampler : register(s0);
SamplerState BrdfSampler : register(s2);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 albedoColor = AlbedoTexture.Sample(DefaultSampler, input.TextureCoord).rgb;
    float3 normalColor = NormalTexture.Sample(DefaultSampler, input.TextureCoord).rgb;
    float4 metalicSmoothnessColor = MetalicSmoothnessTexture.Sample(DefaultSampler, input.TextureCoord);
    float occlusionColor = OcclusionTexture.Sample(DefaultSampler, input.TextureCoord).r;
    float3 emissionColor = EmissionTexture.Sample(DefaultSampler, input.TextureCoord).rgb;

    float3 calculatedNormal = calculateNormal(normalColor, input.TBN);
    float roughness = 1 - metalicSmoothnessColor.a;
    float3 lightColor = DirectionalLightColor * DirectionalLightStrength;
    
    input.LightSpacePosition.xyz /= input.LightSpacePosition.w;
    input.LightSpacePosition.x = input.LightSpacePosition.x / 2 + 0.5f;
    input.LightSpacePosition.y = input.LightSpacePosition.y / -2 + 0.5f;
    
    input.LightSpacePosition.z -= 0.005;
    
    float shadowMapDepth = ShadowMap.Sample(DefaultSampler, input.LightSpacePosition.xy);
    if (shadowMapDepth < input.LightSpacePosition.z)
        albedoColor *= 0.01f;

    float3 finalColor = pbr(albedoColor, calculatedNormal, metalicSmoothnessColor.r, roughness, occlusionColor,
                            IrradianceTexture, RadianceTexture, BrdfLut, DefaultSampler, BrdfSampler,
                            DirectionalLightDirection, lightColor, CameraPosition, input.WorldPosition);

    return float4(finalColor + emissionColor, Alpha);
}