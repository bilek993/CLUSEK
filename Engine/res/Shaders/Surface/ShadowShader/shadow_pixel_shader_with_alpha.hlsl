cbuffer DiscardPixelsBuffer : register(b0)
{
    float ThresholdAlpha;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D AlbedoTexture : register(t0);

SamplerState DefaultSampler : register(s0);

void main(PS_INPUT input)
{
    float alpha = AlbedoTexture.Sample(DefaultSampler, input.TextureCoord).a;
    if (alpha < ThresholdAlpha)
        discard;
}