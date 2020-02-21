float3x3 CalculateTBN(float3 normal, float3 tangent)
{
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float3 bitangent = cross(normal, tangent);

    return float3x3(tangent, bitangent, normal);
}

float3x3 CalculateTBN(float3 tangent, float3 bitangent, float3 normal)
{
    return float3x3(tangent, bitangent, normal);
}

float3 CalculateNormal(float3 normalMap, float3x3 TBN)
{
    normalMap = (2.0f * normalMap) - 1.0f;
    normalMap.g *= -1.0f;

    return normalize(mul(normalMap, TBN));
}

void CalculateNormalForTerrain( float2 textureCoord,
                                Texture2D heightTexture,
                                SamplerState heightTextureSamplerState,
                                float worldCellSpace,
                                float texelSpace,
                                float maxHeight,
                                out float3 tangent,
                                out float3 bitangent,
                                out float3 normal)
{
    float2 leftTexel = textureCoord + float2(-texelSpace, 0.0f);
    float2 rightTexel = textureCoord + float2(texelSpace, 0.0f);
    float2 bottomTexel = textureCoord + float2(0.0f, texelSpace);
    float2 topTexel = textureCoord + float2(0.0f, -texelSpace);
    
    float leftY = heightTexture.SampleLevel(heightTextureSamplerState, leftTexel, 0).r * maxHeight;
    float rightY = heightTexture.SampleLevel(heightTextureSamplerState, rightTexel, 0).r * maxHeight;
    float bottomY = heightTexture.SampleLevel(heightTextureSamplerState, bottomTexel, 0).r * maxHeight;
    float topY = heightTexture.SampleLevel(heightTextureSamplerState, topTexel, 0).r * maxHeight;
    
    tangent = normalize(float3(2.0f * worldCellSpace, rightY - leftY, 0.0f));
    bitangent = normalize(float3(0.0f, bottomY - topY, -2.0f * worldCellSpace));
    normal = cross(tangent, bitangent);
}
