#include "../../Includes/grass_instance_buffer.hlsli"

struct PS_INPUT
{
    float4 Position : SV_POSITION;
};

StructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(t0);

float4 main(PS_INPUT input) : SV_TARGET
{
	// TODO: Add implementation code
    return float4(1.0, 0.0, 0.0f, 1.0f);
}