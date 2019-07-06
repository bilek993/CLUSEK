cbuffer ConstBuffer : register(b0)
{
    float4x4 Mat;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 TextureCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 TextureCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), Mat);
    output.TextureCoord = input.TextureCoord;

    return output;
}