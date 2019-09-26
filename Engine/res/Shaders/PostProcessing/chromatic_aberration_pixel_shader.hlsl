struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

static const float SIZE = 0.00025f;

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = InputTexture.Sample(Sampler, input.TextureCoord).rgb;

    output.r = InputTexture.Sample(Sampler, input.TextureCoord + float2(SIZE, 0.0f)).r;
    output.b = InputTexture.Sample(Sampler, input.TextureCoord - float2(SIZE, 0.0f)).b;

    return float4(output, 1.0f);
}