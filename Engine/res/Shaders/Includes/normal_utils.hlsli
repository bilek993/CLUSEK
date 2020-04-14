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

float3 CalculateNormal(float2 normalMap, float3x3 TBN)
{
    normalMap = (2.0f * normalMap) - 1.0f;
    normalMap.g *= -1.0f;
    
    float reconstructZ = sqrt(1.0f - saturate(dot(normalMap, normalMap)));
    float3 reconstructedNormal = float3(normalMap.xy, reconstructZ);
    reconstructedNormal = normalize(reconstructedNormal);

    return normalize(mul(reconstructedNormal, TBN));
}