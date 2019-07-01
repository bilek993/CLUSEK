struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D Texture : TEXTURE : register(t0);
SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 pixelColor = Texture.Sample(Sampler, input.TextureCoord);
    return float4(pixelColor, 1.0f);
}