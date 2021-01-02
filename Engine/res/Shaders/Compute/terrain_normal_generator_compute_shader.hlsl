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
        leftTexel.x = 0;
    if (rightTexel.x >= outputWidth)
        rightTexel.x -= outputWidth - 1;
    if (topTexel.y < 0)
        topTexel.y = 0;
    if (bottomTexel.y >= outputWidth)
        bottomTexel.y = outputWidth - 1;
    
    float centerY = HeightmapTexture.Load(int3(threadID, 0)).r * MaxHeight;
    float leftY = HeightmapTexture.Load(int3(leftTexel, 0)).r * MaxHeight;
    float rightY = HeightmapTexture.Load(int3(rightTexel, 0)).r * MaxHeight;
    float topY = HeightmapTexture.Load(int3(topTexel, 0)).r * MaxHeight;
    float bottomY = HeightmapTexture.Load(int3(bottomTexel, 0)).r * MaxHeight;
    
    float3 center = float3(threadID.x * WorldCellSpace, centerY, threadID.y * WorldCellSpace);
    float3 left = float3(leftTexel.x * WorldCellSpace, leftY, leftTexel.y * WorldCellSpace);
    float3 right = float3(rightTexel.x * WorldCellSpace, rightY, rightTexel.y * WorldCellSpace);
    float3 top = float3(topTexel.x * WorldCellSpace, topY, topTexel.y * WorldCellSpace);
    float3 bottom = float3(bottomTexel.x * WorldCellSpace, bottomY, bottomTexel.y * WorldCellSpace);
    
    float3 v1 = normalize(left - center);
    float3 v2 = normalize(right - center);
    float3 v3 = normalize(top - center);
    float3 v4 = normalize(bottom - center);
	float3 v5 = normalize(center - left);
    
    float3 normal = float3(0.0f, 0.0f, 0.0f);
    normal += cross(v1, v3);
    normal += cross(v3, v2);
    normal += cross(v2, v4);
    normal += cross(v4, v1);
    normal /= 4;
    normal *= -1;
    
    float3 tangent = float3(0.0f, 0.0f, 0.0f);
    tangent += v2;
    tangent += v5;
    tangent /= 2;
    tangent *= -1;
    
    OutputNormalTexture[threadID] = float4(normal, 0.0f);
    OutputTangentTexture[threadID] = float4(tangent, 0.0f);
}