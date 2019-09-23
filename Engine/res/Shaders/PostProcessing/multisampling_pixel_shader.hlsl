struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2DMS<float4> InputTexture : register(t0);
SamplerState Sampler : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float inputMipLevel, inputHeight, numberOfSamples;
    InputTexture.GetDimensions(inputMipLevel, inputHeight, numberOfSamples);

    float4 colorSum = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < numberOfSamples; i++)
    {
        colorSum += InputTexture.Load(input.Position.xy, i);
    }

    return colorSum / numberOfSamples;
}