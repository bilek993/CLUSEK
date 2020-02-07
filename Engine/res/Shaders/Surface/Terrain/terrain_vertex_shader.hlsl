#include "../../Includes/normal_utils.hlsli"
#include "../../Includes/shadow_utils.hlsli"

cbuffer FatPerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
    float4x4 WorldMatrix;
    float4x4 LightSpaceMatrix[CASCADES_COUNT];
};


struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float2 BoundsY : BOUNDS_Y;
};

struct VS_OUTPUT
{
    float3 Position : POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float2 TextureCoord : TEXCOORD;
    float2 BoundsY : BOUNDS_Y;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    output.WorldPosition = mul(float4(input.Position, 1.0f), WorldMatrix);
    output.TextureCoord = input.TextureCoord;
    output.BoundsY.x = mul(float4(0.0f, input.BoundsY.x, 0.0f, 1.0f), WorldMatrix).y;
    output.BoundsY.y = mul(float4(0.0f, input.BoundsY.y, 0.0f, 1.0f), WorldMatrix).y;

    return output;
}