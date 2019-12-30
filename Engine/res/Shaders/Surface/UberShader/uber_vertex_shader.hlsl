#include "../../Includes/normal_utils.hlsli"

cbuffer FatPerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
    float4x4 WorldMatrix;
    float4x4 LightSpaceMatrix;
    float3x3 InvertedWorldMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3x3 TBN : TBN;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), WorldViewProjectionMat);
    output.WorldPosition = mul(float4(input.Position, 1.0f), WorldMatrix).xyz;
    output.LightSpacePosition = mul(float4(output.WorldPosition, 1.0f), LightSpaceMatrix);
    output.TextureCoord = input.TextureCoord;
    output.Normal = mul(input.Normal, InvertedWorldMatrix);
    output.TBN = calculateTBN(mul(float4(input.Normal, 0.0f), WorldMatrix).xyz, (mul(float4(input.Tangent, 0.0f), WorldMatrix).xyz));

    return output;
}