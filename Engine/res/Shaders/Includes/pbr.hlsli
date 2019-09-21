static const float PI = 3.14159265359f;
static const float3 DIELECTRIC_FACTOR = float3(0.04f, 0.04f, 0.04f);
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
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float(1.0f - roughness).xxx, F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

int getTextureMipMapLevels(TextureCube input)
{
    int width, heigth, levels;
    input.GetDimensions(0, width, heigth, levels);
    return levels;
}

float3 pbr(float3 albedo, float3 normal, float metallic, float roughness, float occlusion,
          TextureCube irradianceTexture, TextureCube radianceTexture, Texture2D brdfLut, 
          SamplerState defaultSampler, SamplerState brdfSampler, float3 lightDirection, 
          float3 lightColor, float3 cameraPosition, float3 pixelPosition)
{
    lightDirection *= -1;

    float3 viewDirection = normalize(cameraPosition - pixelPosition);
    float3 halfwayDirection = normalize(viewDirection + lightDirection);
    float3 reflectionDirection = reflect(-viewDirection, normal);

    float3 F0 = lerp(DIELECTRIC_FACTOR, albedo, metallic);

    float cosView = max(dot(normal, viewDirection), 0.0f);
    float cosLight = max(dot(normal, lightDirection), 0.0f);

    float NDF = distributionGGX(normal, halfwayDirection, roughness);
    float G = geometrySmith(normal,roughness, cosView, cosLight);
    float3 F = fresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0f), F0);

    float3 nominator = NDF * G * F;
    float denominator = 4 * cosView * cosLight + EPSILON;
    float3 specular = nominator / denominator;

    float3 kD = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), metallic);

    float3 directLighting = (kD * albedo / PI + specular) * lightColor * cosLight;

    F = fresnelSchlickRoughness(max(dot(normal, viewDirection), 0.0f), F0, roughness);
    kD = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), metallic);

    float3 irradiance = irradianceTexture.Sample(defaultSampler, normal).rgb;
    float3 diffuse = irradiance * albedo;

    int radianceLevels = getTextureMipMapLevels(radianceTexture);
    float3 radiance = radianceTexture.SampleLevel(defaultSampler, reflectionDirection, roughness * radianceLevels).rgb;
    float2 brdf = brdfLut.Sample(brdfSampler, float2(max(dot(normal, viewDirection), 0.0f), roughness)).xy;
    float3 specularColor = radiance * (F * brdf.x + brdf.y);

    float3 ambientLighting = (kD * diffuse + specularColor) * occlusion;

    return ambientLighting + directLighting;
}