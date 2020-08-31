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

cbuffer GrassAppearanceParametersBuffer : register(b2)
{
    float AlphaDiscardPoint;
    float AlbedoInterpolationRatio;
    float NormalInterpolationRatio;
    float RoughnessInterpolationTarget;
    float RoughnessInterpolationRatio;
    float MetalicInterpolationTarget;
    float MetalicInterpolationRatio;
    float OcclusionValue;
    float WindVertexAffectHeight;
    float GlobalWindSpeed;
    float GlobalWindScale;
    float LocalWindSpeed;
    float3 LocalWindScale;
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
    if (vertexPosition.y < WindVertexAffectHeight)
        return vertexPosition;

    float timeScaled = Time * GlobalWindSpeed;
	
    float3 padding;
    padding.x = (2.0f * sin(1.0f * (objectPosition.x + objectPosition.y + objectPosition.z + timeScaled))) + 1.0f;
    padding.y = 0.0f;
    padding.z = (1.0f * sin(2.0f * (objectPosition.x + objectPosition.y + objectPosition.z + timeScaled))) + 0.5f;

    padding *= GlobalWindScale;
    
    return vertexPosition + padding;
}

float3 ApplyLocalWind(float3 vertexPosition)
{
    if (vertexPosition.y < WindVertexAffectHeight)
        return vertexPosition;
	
    float timeScaled = Time * LocalWindSpeed;

    float displacement = 0.065f + sin(2.65f * (vertexPosition.x + vertexPosition.y + vertexPosition.z + timeScaled));
    float3 padding = displacement * LocalWindScale;

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