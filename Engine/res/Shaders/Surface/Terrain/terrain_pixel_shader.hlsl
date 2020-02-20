#include "../../Includes/normal_utils.hlsli"

cbuffer TerrainNormalBuffer : register(b0)
{
    float WorldCellSpace;
    float TexelSpace;
    float MaxHeight;
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
    
    float4 splatId = SplatmapTexture.Sample(ClampSampler, input.TextureCoord);
    
    float3 color = BaseAlbedoTexture.Sample(WrapSampler, input.TextureCoord).rgb;
    color = lerp(color, RedAlbedoTexture.Sample(WrapSampler, input.TextureCoord).rgb, splatId.r);
    color = lerp(color, GreenAlbedoTexture.Sample(WrapSampler, input.TextureCoord).rgb, splatId.g);
    color = lerp(color, BlueAlbedoTexture.Sample(WrapSampler, input.TextureCoord).rgb, splatId.b);
    
    float sunColorMultiplier = max(dot(normalize(float3(-0.8f, 0.8f, -0.695f)), normal), 0.15f);
    
    return float4(color * sunColorMultiplier, 1.0f);
}