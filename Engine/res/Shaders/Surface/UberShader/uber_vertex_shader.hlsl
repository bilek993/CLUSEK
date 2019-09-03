cbuffer FatPerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
    float4x4 WorldMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 TextureCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
    float3x3 TBN : TBN;
};

float3x3 calculateTbnMatrix(VS_INPUT input)
{
    float3 tangent = normalize(input.Tangent - dot(input.Tangent, input.Normal) * input.Normal);
    float3 bitangent = cross(input.Normal, tangent);

    return float3x3(tangent, bitangent, input.Normal);
}

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), WorldViewProjectionMat);
    output.TextureCoord = input.TextureCoord;
    output.TBN = calculateTbnMatrix(input);

    return output;
}