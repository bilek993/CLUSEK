#include "../../Includes/shadow_utils.hlsli"
#include "../../Includes/grass_instance_buffer.hlsli"

static const float SAMPLING_THRESHOLD = 0.01f;

cbuffer TerrainUvBuffer : register(b0)
{
    float TexturesScale;
}

cbuffer GrassGeneratorParametersBuffer : register(b1)
{
    float GrassPlacementThreshold;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : WORLD_POSITION;
    float4 LightSpacePosition[CASCADES_COUNT] : LIGHTSPACE_POSITION;
    float2 TextureCoord : TEXCOORD;
    float CameraDistanceZ : CAMERA_DISTANCE_Z;
};

AppendStructuredBuffer<GrassInstanceBuffer> GrassInstanceBufferInstance : register(u0);

Texture2D SplatmapTexture : register(t0);
Texture2D GrassPlacement : register(t1);

Texture2D BaseAlbedoTexture : register(t2);
Texture2D RedAlbedoTexture : register(t3);
Texture2D GreenAlbedoTexture : register(t4);
Texture2D BlueAlbedoTexture : register(t5);

Texture2D OptimizedMetalicTexture : register(t6);
Texture2D OptimizedSmoothnessTexture : register(t7);

Texture2D CalculatedNormalTexture : register(t8);

SamplerState WrapSampler : register(s0);
SamplerState ClampSampler : register(s1);

float3 CalculateAlbedoColor(PS_INPUT input, float3 splatId)
{
    float3 color = BaseAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb;
    if (splatId.r > SAMPLING_THRESHOLD)
        color = lerp(color, RedAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb, splatId.r);
    if (splatId.g > SAMPLING_THRESHOLD)
        color = lerp(color, GreenAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb, splatId.g);
    if (splatId.b > SAMPLING_THRESHOLD)
        color = lerp(color, BlueAlbedoTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale).rgb, splatId.b);
    
    return color;
}

float2 CalculateMetalicSmoothnessColor(PS_INPUT input, float3 splatId)
{
    float4 sampledMetalic = OptimizedMetalicTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale);
    float4 sampledSmoothness = OptimizedSmoothnessTexture.Sample(WrapSampler, input.TextureCoord * TexturesScale);
    
    float2 color = float2(sampledMetalic.a, sampledSmoothness.a);
    color = lerp(color, float2(sampledMetalic.r, sampledSmoothness.r), splatId.r);
    color = lerp(color, float2(sampledMetalic.g, sampledSmoothness.g), splatId.g);
    color = lerp(color, float2(sampledMetalic.b, sampledSmoothness.b), splatId.b);
    
    return color;
}

void main(PS_INPUT input)
{
    if (GrassPlacement.Sample(ClampSampler, input.TextureCoord).r < GrassPlacementThreshold) // TODO: Make this changeable
        discard;
	
    float3 splatId = SplatmapTexture.Sample(ClampSampler, input.TextureCoord).rgb;
	
    float3 albedoColor = CalculateAlbedoColor(input, splatId);
    float2 metalicSmoothnessColor = CalculateMetalicSmoothnessColor(input, splatId);
	
    GrassInstanceBuffer instance = (GrassInstanceBuffer)0;
    instance.Position = input.WorldPosition;
    instance.Smoothness = metalicSmoothnessColor.g;
    instance.AlbedoColor = albedoColor;
    instance.Metalness = metalicSmoothnessColor.r;
    instance.Normal = CalculatedNormalTexture.Sample(ClampSampler, input.TextureCoord).rgb;
	//instance.Rotation = ? TODO: Add rotation calculation
    
    GrassInstanceBufferInstance.Append(instance);
}