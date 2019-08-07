struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 TextureCoord : TEXCOORD;
};

TextureCube SkyMap : TEXTURE : register(t0);
SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    return SkyMap.Sample(Sampler, input.TextureCoord);
}