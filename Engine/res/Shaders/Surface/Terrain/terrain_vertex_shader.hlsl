#include "../../Includes/normal_utils.hlsli"
#include "../../Includes/shadow_utils.hlsli"

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    output.TextureCoord = input.TextureCoord;
    output.Normal = input.Normal;
    output.Tangent = input.Tangent;

    return output;
}