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
SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);

float3 CalculateColor(PS_INPUT input, float3 splatId)
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
    
    float3 color = CalculateColor(input, splatId);
    
    float sunColorMultiplier = max(dot(normalize(float3(-0.8f, 0.8f, -0.695f)), normal), 0.15f);
    
    return float4(color * sunColorMultiplier, 1.0f);
}