#include "../../Includes/normal_utils.hlsli"
#include "../../Includes/pbr.hlsli"
#include "../../Includes/shadow_utils.hlsli"
#include "../../Includes/fog.hlsli"
#include "../../Includes/lod_utils.hlsli"

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

cbuffer CascadeLevelsBuffer : register(b2)
{
    float4 CascadeEnds;
    float4 Biases;
}

cbuffer FogBuffer : register(b3)
{
    float3 FogColor;
    float FogDensity;
    float3 FogLightColor;
    float FogLightPower;
    float FogMinDistance;
    float SkyConstantValue;
}

cbuffer LodTransitionBuffer : register(b4)
{
    float PercentageCoverage;
    bool InvertedCoverage;
}

cbuffer DiscardPixelsBuffer : register(b5)
{
    float ThresholdAlpha;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3x3 TBN : TBN;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
};

Texture2D AlbedoTexture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D MetalicSmoothnessTexture : register(t2);
Texture2D OcclusionTexture : register(t3);
Texture2D EmissionTexture : register(t4);

Texture2D ShadowMapCascade0 : register(t14);
Texture2D ShadowMapCascade1 : register(t15);
Texture2D ShadowMapCascade2 : register(t16);
Texture2D ShadowMapCascade3 : register(t17);

TextureCube IrradianceTexture : register(t18);
TextureCube RadianceTexture : register(t19);
Texture2D BrdfLut : register(t20);

SamplerState DefaultSampler : register(s0);
SamplerState BrdfSampler : register(s2);
SamplerComparisonState ShadowSampler : register(s3);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 albedoColor = AlbedoTexture.Sample(DefaultSampler, input.TextureCoord);
    if (albedoColor.a < ThresholdAlpha)
        discard;
	
    PerformLodTransition(input.Position, PercentageCoverage, InvertedCoverage, LOD_GRID_SCALE);
	
    float2 normalColor = NormalTexture.Sample(DefaultSampler, input.TextureCoord).rg;
    float4 metalicSmoothnessColor = MetalicSmoothnessTexture.Sample(DefaultSampler, input.TextureCoord);
    float occlusionColor = OcclusionTexture.Sample(DefaultSampler, input.TextureCoord).r;
    float3 emissionColor = EmissionTexture.Sample(DefaultSampler, input.TextureCoord).rgb;

    float3 calculatedNormal = CalculateNormal(normalColor, input.TBN);
    float roughness = 1 - metalicSmoothnessColor.g;
    float3 lightColor = DirectionalLightColor * DirectionalLightStrength;
    
    float shadowMultiplier = 1.0f;
    
    if (input.CameraDistanceZ < CascadeEnds[0])
        shadowMultiplier = CalculateShadows(ShadowMapCascade0, ShadowSampler, input.LightSpacePosition[0], Biases[0]);
    else if (input.CameraDistanceZ < CascadeEnds[1])
        shadowMultiplier = CalculateShadows(ShadowMapCascade1, ShadowSampler, input.LightSpacePosition[1], Biases[1]);
    else if (input.CameraDistanceZ < CascadeEnds[2])
        shadowMultiplier = CalculateShadows(ShadowMapCascade2, ShadowSampler, input.LightSpacePosition[2], Biases[2]);
    else if (input.CameraDistanceZ < CascadeEnds[3])
        shadowMultiplier = CalculateShadows(ShadowMapCascade3, ShadowSampler, input.LightSpacePosition[3], Biases[3]);

    float3 finalColor = Pbr(albedoColor, calculatedNormal, metalicSmoothnessColor.r, roughness, occlusionColor,
                            IrradianceTexture, RadianceTexture, BrdfLut, DefaultSampler, BrdfSampler,
                            DirectionalLightDirection, lightColor, CameraPosition, input.WorldPosition, shadowMultiplier);
    
    finalColor = CalculateFog(finalColor + emissionColor, input.CameraDistanceZ, normalize(CameraPosition - input.WorldPosition),
                              DirectionalLightDirection, FogDensity, FogLightPower, FogMinDistance, FogColor, FogLightColor);

    return float4(finalColor, Alpha);
}