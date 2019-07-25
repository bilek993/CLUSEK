cbuffer PerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
    float4x4 WorldMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), WorldViewProjectionMat);
    output.TextureCoord = input.TextureCoord;
    output.Normal = normalize(mul(float4(input.Normal, 0.0f), WorldMatrix));

    return output;
}