static const float PI = 3.14159265359f;

float3 calculateHalfwayVector(float3 lightDirection, float3 viewPosition, float3 pixelPosition)
{
    float3 viewDirection = normalize(viewPosition - pixelPosition);
    return normalize(lightDirection + viewDirection);
}