float Random(float2 seed, float min, float max)
{
    float2 noise = (frac(sin(dot(seed, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    float randomNumber = saturate(abs(noise.x + noise.y) * 0.5);
    return lerp(min, max, randomNumber);
}

float2 Random(float2 seed, float2 min, float2 max)
{
    float noiseX = (frac(sin(dot(seed, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    float noiseY = sqrt(1 - noiseX * noiseX);
	
    float2 randomNumber = saturate(float2(noiseX, noiseY));
	
    float outputX = lerp(min.x, max.x, randomNumber.x);
    float outputY = lerp(min.y, max.y, randomNumber.y);
	
    return float2(outputX, outputY);
}