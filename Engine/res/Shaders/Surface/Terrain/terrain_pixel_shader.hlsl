#include "../../Includes/normal_utils.hlsli"

cbuffer TerrainNormalBuffer : register(b0)
{
    float WorldCellSpace;
    float TexelSpace;
    float MaxHeight;
}

cbuffer TerrainUvBuffer : register(b1)
{
    float BaseTextureScale;
    float RedTextureScale;
    float GreenTextureScale;
    float BlueTextureScale;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D HeightmapTexture : register(t0);
Texture2D SplatmapTexture : register(t1);

Texture2D BaseAlbedoTexture : register(t2);
Texture2D RedAlbedoTexture : register(t3);
Texture2D GreenAlbedoTexture : register(t4);
Texture2D BlueAlbedoTexture : register(t5);

Texture2D BaseNormalTexture : register(t6);
Texture2D RedNormalTexture : register(t7);
Texture2D GreenNormalTexture : register(t8);
Texture2D BlueNormalTexture : register(t9);

Texture2D BaseMetalicSmoothnessTexture : register(t10);
Texture2D RedMetalicSmoothnessTexture : register(t11);
Texture2D GreenMetalicSmoothnessTexture : register(t12);
Texture2D BlueMetalicSmoothnessTexture : register(t13);

Texture2D BaseOcclusionTexture : register(t14);
Texture2D RedOcclusionTexture : register(t15);
Texture2D GreenOcclusionTexture : register(t16);
Texture2D BlueOcclusionTexture : register(t17);

TextureCube IrradianceTexture : register(t18);
TextureCube RadianceTexture : register(t19);
Texture2D BrdfLut : register(t20);

SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);

float3 CalculateAlbedoColor(PS_INPUT input, float3 splatId)
{
    float3 color = BaseAlbedoTexture.Sample(WrapSampler, input.TextureCoord * BaseTextureScale).rgb;
    color = lerp(color, RedAlbedoTexture.Sample(WrapSampler, input.TextureCoord * RedTextureScale).rgb, splatId.r);
    color = lerp(color, GreenAlbedoTexture.Sample(WrapSampler, input.TextureCoord * GreenTextureScale).rgb, splatId.g);
    color = lerp(color, BlueAlbedoTexture.Sample(WrapSampler, input.TextureCoord * BlueTextureScale).rgb, splatId.b);
    
    return color;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 tangent, bitangent, normal;
    CalculateNormalForTerrain(  input.TextureCoord, 
                                HeightmapTexture, 
                                ClampSampler, 
                                WorldCellSpace, 
                                TexelSpace, 
                                MaxHeight, 
                                tangent, 
                                bitangent, 
                                normal);
    
    float3 splatId = SplatmapTexture.Sample(ClampSampler, input.TextureCoord).rgb;
    
    float3 albedoColor = CalculateAlbedoColor(input, splatId);
    
    float sunColorMultiplier = max(dot(normalize(float3(-0.8f, 0.8f, -0.695f)), normal), 0.15f);
    
    return float4(albedoColor * sunColorMultiplier, 1.0f);
}