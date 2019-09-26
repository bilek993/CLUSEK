struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    return InputTexture.Sample(Sampler, input.TextureCoord);
}