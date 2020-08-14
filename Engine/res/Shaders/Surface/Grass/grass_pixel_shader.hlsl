#include "../../Includes/fog.hlsli"
#include "../../Includes/shadow_utils.hlsli"
#include "../../Includes/grass_instance_buffer.hlsli"
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

cbuffer GrassAppearanceParametersBuffer : register(b4)
{
    float AlphaDiscardPoint;
    float AlbedoInterpolationRatio;
    float NormalInterpolationRatio;
    float RoughnessInterpolationTarget;
    float RoughnessInterpolationRatio;
    float MetalicInterpolationTarget;
    float MetalicInterpolationRatio;
    float OcclusionValue;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
    uint InstanceId : SV_InstanceID;
};

StructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(t0);
Texture2D AlbedoTexture : register(t1);

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
    float4 sampledAlbedoColor = AlbedoTexture.Sample(DefaultSampler, input.TextureCoord);
    if (sampledAlbedoColor.a < AlphaDiscardPoint)
        discard;
	
    GrassInstanceBuffer currentBufferInstance = GrassInstanceBufferInstance[input.InstanceId];
	
    float3 albedo = lerp(sampledAlbedoColor.rgb, currentBufferInstance.AlbedoColor, AlbedoInterpolationRatio);
    float3 calculatedNormal = lerp(input.Normal, currentBufferInstance.Normal, NormalInterpolationRatio);
    float roughness = lerp(1 - GrassInstanceBufferInstance[input.InstanceId].Smoothness, RoughnessInterpolationTarget, RoughnessInterpolationRatio);
    float metallic = lerp(GrassInstanceBufferInstance[input.InstanceId].Metalness, MetalicInterpolationTarget, MetalicInterpolationRatio);
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
	
    float3 finalColor = Pbr(albedo, calculatedNormal, metallic, roughness, OcclusionValue,
                            IrradianceTexture, RadianceTexture, BrdfLut, DefaultSampler, BrdfSampler,
                            DirectionalLightDirection, lightColor, CameraPosition, input.WorldPosition, shadowMultiplier);
    
    finalColor = CalculateFog(finalColor, input.CameraDistanceZ, normalize(CameraPosition - input.WorldPosition),
                              DirectionalLightDirection, FogDensity, FogLightPower, FogMinDistance, FogColor, FogLightColor);

    return float4(finalColor, Alpha);
}