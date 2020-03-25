float3 CalculateFog(float3 inputColor, float distanceFromCamera, float3 viewDirection, float3 lightDirection,
                    float density, float minDistance, float3 fogColor, float3 lightColor)
{
    float fogAmount = 1.0f - exp(-max(distanceFromCamera - minDistance, 0.0f) * density);
    float sunAmount = max(dot(lightDirection, viewDirection), 0.0f);
    
    fogColor = lerp(fogColor, lightColor, pow(sunAmount, 8.0f));
    
    return lerp(inputColor, fogColor, fogAmount);
}