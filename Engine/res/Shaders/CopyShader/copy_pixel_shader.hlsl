struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

//Texture2D InputTexture : TEXTURE : register(t0);
//SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
   // return InputTexture.Sample(Sampler, input.TextureCoord);
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}