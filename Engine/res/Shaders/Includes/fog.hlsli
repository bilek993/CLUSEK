float3 CalculateFog(float3 inputColor, float distanceFromCamera, float density, float minDistance, float3 color)
{
    float fogAmount = 1.0f - exp(-max(distanceFromCamera - minDistance, 0.0f) * density);
    return lerp(inputColor, color, fogAmount);
}