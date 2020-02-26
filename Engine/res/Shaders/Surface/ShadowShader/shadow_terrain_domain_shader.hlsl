#include "../../Includes/shadow_utils.hlsli"

cbuffer ShadowBuffer : register(b0)
{
    float4x4 WorldLightMatrix;
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
};

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

Texture2D HeightmapTexture : register(t0);
SamplerState ClampSampler : register(s1);

[domain("quad")]
DS_OUTPUT main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<DS_INPUT, 4> quad)
{
    float3 position = lerp(lerp(quad[0].Position, quad[1].Position, uv.x), lerp(quad[2].Position, quad[3].Position, uv.x), uv.y);
    float2 coord = lerp(lerp(quad[0].TextureCoord, quad[1].TextureCoord, uv.x), lerp(quad[2].TextureCoord, quad[3].TextureCoord, uv.x), uv.y);
    
    position.y = HeightmapTexture.SampleLevel(ClampSampler, coord, 0).r * MaxHeight;
    
    DS_OUTPUT output;
    output.Position = mul(float4(position, 1.0f), WorldLightMatrix);

    return output;
}