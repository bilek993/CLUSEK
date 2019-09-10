#include "../../Includes/gamma_correction_utils.hlsli"
#include "../../Includes/normal_utils.hlsli"

cbuffer LightAndAlphaBuffer : register(b0)
{
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
    float3 albedoColor = AlbedoTexture.Sample(Sampler, input.TextureCoord).rgb;
    float3 normalColor = NormalTexture.Sample(Sampler, input.TextureCoord).rgb;
    float metalicColor = MetalicSmoothnessTexture.Sample(Sampler, input.TextureCoord).r;
    float smoothnessColor = MetalicSmoothnessTexture.Sample(Sampler, input.TextureCoord).a;
    float occlusionColor = MetalicSmoothnessTexture.Sample(Sampler, input.TextureCoord).r;

    albedoColor = gammaCorrectTexture(albedoColor);

    float3 directionalLight = saturate(dot(DirectionalLightDirection, calculateNormal(normalColor, input.TBN)) * (DirectionalLightColor * DirectionalLightStrength) * albedoColor);

    float3 finalColor = albedoColor + directionalLight;

    return float4(finalColor, Alpha);
}