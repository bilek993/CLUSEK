static const float WIDTH = 0.001f;

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

float3 DrawLine(float currentPosition, float linePosition, float3 color)
{
    if (currentPosition > linePosition - (WIDTH / 2) && currentPosition < linePosition + (WIDTH / 2))
        return float3(1.0f, 1.0f, 1.0f);
    else
        return color;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = InputTexture.Sample(Sampler, input.TextureCoord).rgb;
    
    output = DrawLine(input.TextureCoord.x, 0.3333333333f, output);
    output = DrawLine(input.TextureCoord.x, 0.6666666666f, output);
    
    output = DrawLine(input.TextureCoord.y, 0.3333333333f, output);
    output = DrawLine(input.TextureCoord.y, 0.6666666666f, output);
    
    return float4(output, 1.0f);
}