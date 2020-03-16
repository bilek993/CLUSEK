#include "../../Includes/shadow_utils.hlsli"

cbuffer FatPerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
    float4x4 WorldMatrix;
    float4x4 LightSpaceMatrix[CASCADES_COUNT];
};

cbuffer TerrainHeightSamplingBuffer : register(b1)
{
    float MaxHeight;
}

struct DS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
};

struct DS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
};

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

Texture2D HeightmapTexture : register(t0);
SamplerState TerrainSamplerState : register(s0);

[domain("quad")]
DS_OUTPUT main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<DS_INPUT, 4> quad)
{
    float3 position = lerp(lerp(quad[0].Position, quad[1].Position, uv.x), lerp(quad[2].Position, quad[3].Position, uv.x), uv.y);
    float2 coord = lerp(lerp(quad[0].TextureCoord, quad[1].TextureCoord, uv.x), lerp(quad[2].TextureCoord, quad[3].TextureCoord, uv.x), uv.y);
    
    position.y = HeightmapTexture.SampleLevel(TerrainSamplerState, coord, 0).r * MaxHeight;
    
    DS_OUTPUT output;
    output.Position = mul(float4(position, 1.0f), WorldViewProjectionMat);
    output.WorldPosition = mul(float4(position, 1.0f), WorldMatrix).xyz;
    output.TextureCoord = coord;
    output.CameraDistanceZ = output.Position.z;
    
    for (int i = 0; i < CASCADES_COUNT; i++)
        output.LightSpacePosition[i] = mul(float4(output.WorldPosition, 1.0f), LightSpaceMatrix[i]);

    return output;
}