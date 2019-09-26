struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

static const float2 CENTER = float2(0.5f, 0.5f);
static const float AVOID_DISTANCE = 0.2f;
static const float SIZE = 0.075f;
static const float ATTENUATION = 5.0f;

float calculateIntensity(float2 textureCoord)
{
    float offset = distance(CENTER, textureCoord) - AVOID_DISTANCE;
    offset = saturate(offset);
    offset = pow(offset, ATTENUATION);

    return offset;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = InputTexture.Sample(Sampler, input.TextureCoord).rgb;

    float intensity = calculateIntensity(input.TextureCoord);

    output.r = InputTexture.Sample(Sampler, input.TextureCoord + float2(SIZE * intensity, 0.0f)).r;
    output.b = InputTexture.Sample(Sampler, input.TextureCoord - float2(SIZE * intensity, 0.0f)).b;

    return float4(output, 1.0f);
}