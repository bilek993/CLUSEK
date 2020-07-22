struct GrassInstanceBuffer
{
    float3 Position;
    float Smoothness;
    float3 AlbedoColor;
    float Metalness;
};

cbuffer TerrainUvBuffer : register(b0)
{
    float TexturesScale;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float2 TextureCoord : TEXCOORD;
};

AppendStructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(u0);

Texture2D Texture : register(t0);

SamplerState Sampler : register(s0);

void main(PS_INPUT input)
{
    GrassInstanceBuffer instance = (GrassInstanceBuffer)0;
    instance.Position = input.Position;
    
    GrassInstanceBufferInstance.Append(instance);
}