float3 CalculateFog(float3 inputColor, float distanceFromCamera, float density, float3 color)
{
    float fogAmount = 1.0f - exp(-distanceFromCamera * density);
    return lerp(inputColor, color, fogAmount);
}