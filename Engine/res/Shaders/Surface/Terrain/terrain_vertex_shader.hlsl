#include "../../Includes/normal_utils.hlsli"
#include "../../Includes/shadow_utils.hlsli"

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float2 BoundsY : BOUNDS_Y;
};

struct VS_OUTPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float2 BoundsY : BOUNDS_Y;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    output.TextureCoord = input.TextureCoord;
    output.BoundsY = input.BoundsY;

    return output;
}