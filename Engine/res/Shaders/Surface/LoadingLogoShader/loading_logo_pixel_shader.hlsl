struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    return Texture.Sample(Sampler, input.TextureCoord);
}