static const float FOG_DENSITY = 0.00025f;
static const float3 FOG_COLOR = float3(0.3f, 0.6f, 0.7f);

float3 CalculateFog(float3 inputColor, float distanceFromCamera)
{
    float fogAmount = 1.0f - exp(-distanceFromCamera * FOG_DENSITY);
    return lerp(inputColor, FOG_COLOR, fogAmount);
}