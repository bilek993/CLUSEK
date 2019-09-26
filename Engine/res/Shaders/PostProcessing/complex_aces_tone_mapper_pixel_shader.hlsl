struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

static const float3x3 ACES_INPUT_MAT =
{
    { 0.59719, 0.35458, 0.04823 },
    { 0.07600, 0.90834, 0.01566 },
    { 0.02840, 0.13383, 0.83777 }
};

static const float3x3 ACES_OUTPUT_MAT =
{
    { 1.60475, -0.53108, -0.07367 },
    { -0.10208, 1.10813, -0.00605 },
    { -0.00327, -0.07276, 1.07602 }
};

float3 RRTAndODTFit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = InputTexture.Sample(Sampler, input.TextureCoord).rgb;

    output = mul(ACES_INPUT_MAT, output);
    output = RRTAndODTFit(output);
    output = mul(ACES_OUTPUT_MAT, output);
    output = saturate(output);

    return float4(output, 1.0f);
}