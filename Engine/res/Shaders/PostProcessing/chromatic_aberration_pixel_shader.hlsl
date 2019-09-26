struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 output = InputTexture.Sample(Sampler, input.TextureCoord).rgb;
    return float4(output, 1.0f);
}