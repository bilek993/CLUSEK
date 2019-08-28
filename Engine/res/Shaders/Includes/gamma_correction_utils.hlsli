float3 gammaCorrectTexture(float3 colorInput)
{
    return pow(abs(colorInput), 2.2f);
}

float3 gammaCorrectFinal(float3 colorInput)
{
    return pow(abs(colorInput), 1.0f / 2.2f);
}