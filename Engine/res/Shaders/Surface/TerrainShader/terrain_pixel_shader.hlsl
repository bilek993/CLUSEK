#include "../../Includes/normal_utils.hlsli"
#include "../../Includes/pbr.hlsli"
#include "../../Includes/shadow_utils.hlsli"
#include "../../Includes/fog.hlsli"

static const float SAMPLING_THRESHOLD = 0.01f;

cbuffer TerrainUvBuffer : register(b0)
{
    float TexturesScale;
}

cbuffer LightAndAlphaBuffer : register(b1)
{
    float3 DirectionalLightColor;
    float DirectionalLightStrength;
    float3 DirectionalLightDirection;
    float Alpha;
}

cbuffer CameraBuffer : register(b2)
{
    float3 CameraPosition;
}

cbuffer CascadeLevelsBuffer : register(b3)
{
    float4 CascadeEnds;
    float4 Biases;
}

cbuffer FogBuffer : register(b4)
{
    float3 FogColor;
    float FogDensity;
    float3 FogLightColor;
    float FogLightPower;
    float FogMinDistance;
    float SkyConstantValue;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
};

Texture2D SplatmapTexture : register(t0);

Texture2D BaseAlbedoTexture : register(t1);
Texture2D RedAlbedoTexture : register(t2);
Texture2D GreenAlbedoTexture : register(t3);
Texture2D BlueAlbedoTexture : register(t4);

Texture2D BaseNormalTexture : register(t5);
Texture2D RedNormalTexture : register(t6);
Texture2D GreenNormalTexture : register(t7);
Texture2D BlueNormalTexture : register(t8);

Texture2D OptimizedOcclusionTexture : register(t9);
Texture2D OptimizedMetalicTexture : register(t10);
Texture2D OptimizedSmoothnessTexture : register(t11);

Texture2D CalculatedNormalTexture : register(t12);
Texture2D CalculatedTangentTexture : register(t13);

Texture2D ShadowMapCascade0 : register(t14);
Texture2D ShadowMapCascade1 : register(t15);
Texture2D ShadowMapCascade2 : register(t16);
Texture2D ShadowMapCascade3 : register(t17);

TextureCube IrradianceTexture : register(t18);
TextureCube RadianceTexture : register(t19);
Texture2D BrdfLut : register(t20);

SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);
SamplerState BrdfSampler : register(s2);
SamplerComparisonState ShadowSampler : register(s3);

float3 CalculateAlbedoColor(PS_INPUT input, float3 splatId)
{
    float3 color = BaseAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb;
    if (splatId.r > SAMPLING_THRESHOLD)
        color = lerp(color, RedAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb, splatId.r);
    if (splatId.g > SAMPLING_THRESHOLD)
        color = lerp(color, GreenAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb, splatId.g);
    if (splatId.b > SAMPLING_THRESHOLD)
        color = lerp(color, BlueAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb, splatId.b);
    
    return color;
}

float3 CalculateNormalColor(PS_INPUT input, float3 splatId)
{
    float2 color = BaseNormalTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rg;
    if (splatId.r > SAMPLING_THRESHOLD)
        color = lerp(color, RedNormalTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rg, splatId.r);
    if (splatId.g > SAMPLING_THRESHOLD)
        color = lerp(color, GreenNormalTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rg, splatId.g);
    if (splatId.b > SAMPLING_THRESHOLD)
        color = lerp(color, BlueNormalTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rg, splatId.b);
    
    return ReconstructNormalZ(color);
}

float2 CalculateMetalicSmoothnessColor(PS_INPUT input, float3 splatId)
{
    float4 sampledMetalic = OptimizedMetalicTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale);
    float4 sampledSmoothness = OptimizedSmoothnessTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale);
    
    float2 color = float2(sampledMetalic.a, sampledSmoothness.a);
    color = lerp(color, float2(sampledMetalic.r, sampledSmoothness.r), splatId.r);
    color = lerp(color, float2(sampledMetalic.g, sampledSmoothness.g), splatId.g);
    color = lerp(color, float2(sampledMetalic.b, sampledSmoothness.b), splatId.b);
    
    return color;
}

float CalculateOcclusionColor(PS_INPUT input, float3 splatId)
{
    float4 sampledOcclusion = OptimizedOcclusionTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale);
    
    float color = sampledOcclusion.a;
    color = lerp(color, sampledOcclusion.r, splatId.r);
    color = lerp(color, sampledOcclusion.g, splatId.g);
    color = lerp(color, sampledOcclusion.b, splatId.b);
    
    return color;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 normal = CalculatedNormalTexture.Sample(ClampSampler, input.TextureCoord).rgb;
    float3 tangent = CalculatedTangentTexture.Sample(ClampSampler, input.TextureCoord).rgb;
    float3 bitangent = cross(normal, tangent);

    float3x3 TBN = CalculateTBN(tangent, bitangent, normal);
    
    float3 splatId = SplatmapTexture.Sample(ClampSampler, input.TextureCoord).rgb;
    
    float3 albedoColor = CalculateAlbedoColor(input, splatId);
    float3 normalColor = CalculateNormalColor(input, splatId);
    float2 metalicSmoothnessColor = CalculateMetalicSmoothnessColor(input, splatId);
    float occlusionColor = CalculateOcclusionColor(input, splatId);
    
    float3 calculatedNormal = CalculateNormal(normalColor, TBN);
    float roughness = 1 - metalicSmoothnessColor.y;
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
                            IrradianceTexture, RadianceTexture, BrdfLut, WrapSampler, BrdfSampler,
                            DirectionalLightDirection, lightColor, CameraPosition, input.WorldPosition, shadowMultiplier);
    
    finalColor = CalculateFog(finalColor, input.CameraDistanceZ, normalize(CameraPosition - input.WorldPosition),
                              DirectionalLightDirection, FogDensity, FogLightPower, FogMinDistance, FogColor, FogLightColor);
    
    return float4(finalColor, Alpha);
}