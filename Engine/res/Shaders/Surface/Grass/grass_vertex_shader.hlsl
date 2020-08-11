#include "../../Includes/shadow_utils.hlsli"

cbuffer GrassPerObjectBuffer : register(b0)
{
    float4x4 ViewProjectionMatrix;
    float4x4 LightSpaceMatrix[CASCADES_COUNT];
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    uint InstanceId : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

float4x4 CalculateWorldMatrix(VS_INPUT input)
{
    return float4x4(
		float4(1, 0, 0, 0),
		float4(0, 1, 0, 0),
	    float4(0, 0, 1, 0),
	    float4(0, 0, 0, 1)
	);
}

VS_OUTPUT main(VS_INPUT input)
{
    float4x4 worldMatrix = CalculateWorldMatrix(input);
    float4x4 worldViewProjectionMatrix = mul(transpose(worldMatrix), ViewProjectionMatrix);
	
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), worldViewProjectionMatrix);
    return output;
}