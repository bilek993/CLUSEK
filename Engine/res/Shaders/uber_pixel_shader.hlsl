cbuffer LightBuffer : register(b0)
{
    float3 AmbientLightColor;
    float AmbientLightStrength;
    float3 DirectionalLightColor;
    float DirectionalLightStrength;
    float3 DirectionalLightDirection;
}

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

Texture2D MainTexture : TEXTURE : register(t0);
SamplerState Sampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 samplerColor = MainTexture.Sample(Sampler, input.TextureCoord);

    float3 ambientLight = AmbientLightColor * AmbientLightStrength;
    float3 directionalLight = saturate(dot(DirectionalLightDirection, input.Normal) * (DirectionalLightColor * DirectionalLightStrength) * samplerColor);

    float3 finalColor = ambientLight * samplerColor;
    finalColor += directionalLight;

    return float4(finalColor, 1.0f);
}