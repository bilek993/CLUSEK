cbuffer SimplePerObjectBuffer : register(b0)
{
    float4x4 WorldViewProjectionMat;
};

struct VS_INPUT
{
    float3 Position : POSITION;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 TextureCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), WorldViewProjectionMat).xyww;
    output.TextureCoord = input.Position.xy + 0.5f;
    output.TextureCoord.y *= -1;

    return output;
}