cbuffer TerrainNormalBuffer : register(b0)
{
    float WorldCellSpace;
    float TexelSpace;
    float MaxHeight;
}


struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D HeightmapTexture : register(t0);
SamplerState ClampSampler : register(s1);

void CalculateNormalTerrain(PS_INPUT input,
                            out float3 tangent,
                            out float3 bitangent, 
                            out float3 normal)
{
    float2 leftTexel = input.TextureCoord + float2(-TexelSpace, 0.0f);
    float2 rightTexel = input.TextureCoord + float2(TexelSpace, 0.0f);
    float2 bottomTexel = input.TextureCoord + float2(0.0f, TexelSpace);
    float2 topTexel = input.TextureCoord + float2(0.0f, -TexelSpace);
    
    float leftY = HeightmapTexture.SampleLevel(ClampSampler, leftTexel, 0).r * MaxHeight;
    float rightY = HeightmapTexture.SampleLevel(ClampSampler, rightTexel, 0).r * MaxHeight;
    float bottomY = HeightmapTexture.SampleLevel(ClampSampler, bottomTexel, 0).r * MaxHeight;
    float topY = HeightmapTexture.SampleLevel(ClampSampler, topTexel, 0).r * MaxHeight;
    
    tangent = normalize(float3(2.0f * WorldCellSpace, rightY - leftY, 0.0f));
    bitangent = normalize(float3(0.0f, bottomY - topY, -2.0f * WorldCellSpace));
    normal = cross(tangent, bitangent);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 tangent, bitangent, normal;
    CalculateNormalTerrain(input, tangent, bitangent, normal);
    
    float color = max(dot(normalize(float3(-0.8f, 0.8f, -0.695f)), normal), 0.15f);
    
    return float4(color.rrr, 1.0f);
}