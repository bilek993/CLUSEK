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
    
    float color = max(dot(normalize(float3(-0.8f, 0.8f, -0.695f)), normal), 0.15f);
    
    return float4(color.rrr, 1.0f);
}