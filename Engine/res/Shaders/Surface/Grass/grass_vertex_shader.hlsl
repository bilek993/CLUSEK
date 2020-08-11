#include "../../Includes/shadow_utils.hlsli"

struct GrassInstanceBuffer
{
    float3 Position;
    float Smoothness;
    float3 AlbedoColor;
    float Metalness;
};

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

float4x4 CalculateWorldMatrix(GrassInstanceBuffer currentBuffer)
{
    return float4x4(
		float4(1, 0, 0, currentBuffer.Position.x),
		float4(0, 1, 0, currentBuffer.Position.y),
	    float4(0, 0, 1, currentBuffer.Position.z),
	    float4(0, 0, 0, 1)
	);
}

StructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(t0);

VS_OUTPUT main(VS_INPUT input)
{
    float4x4 worldMatrix = transpose(CalculateWorldMatrix(GrassInstanceBufferInstance[input.InstanceId]));
    float4x4 worldViewProjectionMatrix = mul(worldMatrix, ViewProjectionMatrix);
	
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), worldViewProjectionMatrix);
    return output;
}