#include "../../Includes/shadow_utils.hlsli"
#include "../../Includes/grass_instance_buffer.hlsli"

cbuffer GrassPerObjectBuffer : register(b0)
{
    float4x4 ViewProjectionMatrix;
    float4x4 LightSpaceMatrix[CASCADES_COUNT];
};

cbuffer TimeBuffer : register(b1)
{
    float Time;
}

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
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
    uint InstanceId : SV_InstanceID;
};

float4x4 CalculateWorldMatrix(GrassInstanceBuffer currentBuffer)
{
    float x = currentBuffer.Position.x;
    float y = currentBuffer.Position.y;
    float z = currentBuffer.Position.z;

    float s = sin(currentBuffer.Rotation);
    float c = cos(currentBuffer.Rotation);
	
    return float4x4(
		float4( c, 0, s, x),
		float4( 0, 1, 0, y),
	    float4(-s, 0, c, z),
	    float4( 0, 0, 0, 1)
	);
}

float3 ApplyGlobalWind(float3 vertexPosition, float3 objectPosition)
{
    if (vertexPosition.y < 0.5f) // TODO: Change this
        return vertexPosition;

    float timeScaled = Time * 0.001f; // TODO: Change this
	
    float3 padding;
    padding.x = (2.0f * sin(1.0f * (objectPosition.x + objectPosition.y + objectPosition.z + timeScaled))) + 1.0f;
    padding.y = 0.0f;
    padding.z = (1.0f * sin(2.0f * (objectPosition.x + objectPosition.y + objectPosition.z + timeScaled))) + 0.5f;

    padding *= 0.5f; // TODO: Change this
    
    return vertexPosition + padding;
}

float3 ApplyLocalWind(float3 vertexPosition)
{
    if (vertexPosition.y < 0.5f) // TODO: Change this
        return vertexPosition;
	
    float timeScaled = Time * 0.001f; // TODO: Change this

    float displacement = 0.065f + sin(2.65f * (vertexPosition.x + vertexPosition.y + vertexPosition.z + timeScaled));
    float3 padding = float3(displacement, displacement * 0.35f, displacement) * 0.5f; // TODO: Change this

    return vertexPosition + padding;
}

StructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(t0);

VS_OUTPUT main(VS_INPUT input)
{
    float4x4 worldMatrix = transpose(CalculateWorldMatrix(GrassInstanceBufferInstance[input.InstanceId]));
    float4x4 worldViewProjectionMatrix = mul(worldMatrix, ViewProjectionMatrix);

    float3 windAppliedPosition = ApplyGlobalWind(input.Position, GrassInstanceBufferInstance[input.InstanceId].Position);
    windAppliedPosition = ApplyLocalWind(windAppliedPosition);
	
    VS_OUTPUT output;
    output.Position = mul(float4(windAppliedPosition, 1.0f), worldViewProjectionMatrix);
    output.WorldPosition = mul(float4(windAppliedPosition, 1.0f), worldMatrix).xyz;
    output.TextureCoord = input.TextureCoord;
    output.Normal = input.Normal;
    output.CameraDistanceZ = output.Position.z;
    output.InstanceId = input.InstanceId;

    for (int i = 0; i < CASCADES_COUNT; i++)
        output.LightSpacePosition[i] = mul(float4(output.WorldPosition, 1.0f), LightSpaceMatrix[i]);
	
    return output;
}