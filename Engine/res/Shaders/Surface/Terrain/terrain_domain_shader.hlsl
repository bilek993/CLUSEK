#include "../../Includes/shadow_utils.hlsli"

cbuffer FatPerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
    float4x4 WorldMatrix;
    float4x4 LightSpaceMatrix[CASCADES_COUNT];
};

struct DS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct DS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

[domain("quad")]
DS_OUTPUT main(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<DS_INPUT, 4> quad) // TODO: Change this logic
{
    DS_OUTPUT output;
    output.Position = mul(float4(quad[0].Position, 1.0f), WorldViewProjectionMat);
    output.TextureCoord = quad[0].TextureCoord;
    output.Normal = quad[0].Normal;
    output.Tangent = quad[0].Tangent;

    return output;
}