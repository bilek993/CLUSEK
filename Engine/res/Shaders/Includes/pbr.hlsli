static const float PI = 3.14159265359f;
static const float3 DIELECTRIC_FACTOR = 0.04;
static const float EPSILON = 0.00001f;

float distributionGGX(float3 normal, float3 halfway, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSquare = alpha * alpha;
    float cosHalfway = max(dot(normal, halfway), 0.0f);
    float cosHalfwaySquare = cosHalfway * cosHalfway;

    float denominator = (cosHalfwaySquare * (alphaSquare - 1.0f)) +1.0f;
    denominator = PI * denominator * denominator;

    return alphaSquare / denominator;
}

float geometrySchlickGGX(float cosinus, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float denominator = cosinus * (1.0 - k) + k;

    return cosinus / denominator;
}

float geometrySmith(float3 normal, float roughness, float cosView, float cosLight)
{
    return geometrySchlickGGX(cosView, roughness) * geometrySchlickGGX(cosLight, roughness);
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f);
}

float3 pbr(float3 albedo, float3 normal, float metallic, float roughness, float occlusion,
          float3 lightDirection, float3 lightColor, float3 cameraPosition, float3 pixelPosition)
{
    float3 viewDirection = normalize(cameraPosition - pixelPosition);
    float3 halfwayDirection = normalize(viewDirection + lightDirection);

    float3 F0 = lerp(DIELECTRIC_FACTOR, albedo, metallic);

    float cosView = max(dot(normal, viewDirection), 0.0f);
    float cosLight = max(dot(normal, lightDirection), 0.0f);

    float NDF = distributionGGX(normal, halfwayDirection, roughness);
    float G = geometrySmith(normal,roughness, cosView, cosLight);
    float3 F = fresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0f), F0);

    float3 nominator = NDF * G * F;
    float denominator = 4 * cosView * cosLight + EPSILON;
    float3 specular = nominator / denominator;

    float3 kd = lerp(float3(1, 1, 1) - F, float3(0.0f, 0.0f, 0.0f), metallic);

    float3 directLighting = (kd * albedo / PI + specular) * lightColor * cosLight;

    // TODO: Add IBL from bellow
    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * occlusion;

    return ambient + directLighting;
}