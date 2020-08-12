#include "../../Includes/shadow_utils.hlsli"
#include "../../Includes/grass_instance_buffer.hlsli"

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
    uint InstanceId : SV_InstanceID;
};

StructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(t0);

float4 main(PS_INPUT input) : SV_TARGET
{
	// TODO: Add implementation code
    return float4(GrassInstanceBufferInstance[input.InstanceId].AlbedoColor, 1.0f);
}