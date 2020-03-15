cbuffer TerrainNormalBuffer : register(b0)
{
    float WorldCellSpace;
    float MaxHeight;
}

Texture2D HeightmapTexture : register(t0);

RWTexture2D<float4> OutputNormalTexture : register(u0);
RWTexture2D<float4> OutputTangentTexture : register(u1);

[numthreads(32, 32, 1)]
void main(uint2 threadID : SV_DispatchThreadID)
{
    float outputWidth, outputHeight;
    OutputNormalTexture.GetDimensions(outputWidth, outputHeight);
    
    float2 leftTexel = threadID + uint2(-1, 0);
    float2 rightTexel = threadID + float2(1, 0);
    float2 topTexel = threadID + float2(0, -1);
    float2 bottomTexel = threadID + float2(0, 1);
    
    if (leftTexel.x < 0)
        leftTexel.x += 1;
    if (rightTexel.x >= outputWidth)
        rightTexel.x -= 1;
    if (topTexel.y < 0)
        topTexel.y += 1;
    if (bottomTexel.y >= outputWidth)
        bottomTexel.y -= 1;
    
    float leftY = HeightmapTexture.Load(int3(leftTexel, 0)).r * MaxHeight;
    float rightY = HeightmapTexture.Load(int3(rightTexel, 0)).r * MaxHeight;
    float topY = HeightmapTexture.Load(int3(topTexel, 0)).r * MaxHeight;
    float bottomY = HeightmapTexture.Load(int3(bottomTexel, 0)).r * MaxHeight;
    
    float3 tangent = normalize(float3(2.0f * WorldCellSpace, rightY - leftY, 0.0f));
    float3 bitangent = normalize(float3(0.0f, bottomY - topY, -2.0f * WorldCellSpace));
    float3 normal = cross(tangent, bitangent);
    
    OutputNormalTexture[threadID] = float4(normal, 1.0f);
    OutputTangentTexture[threadID] = float4(tangent, 1.0f);
}