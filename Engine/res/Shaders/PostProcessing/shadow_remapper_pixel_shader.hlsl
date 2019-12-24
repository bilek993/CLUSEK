cbuffer LightAndAlphaBuffer : register(b0)
{
    float NearZ;
    float FarZ;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D InputTexture : register(t0);
SamplerState Sampler : register(s1);

float4 main(PS_INPUT input) : SV_TARGET
{
    float depth = InputTexture.Sample(Sampler, input.TextureCoord).r;
    depth = (2.0 * NearZ) / (FarZ + NearZ - depth * (FarZ - NearZ));
    
    return float4(depth.rrr, 1.0f);
}