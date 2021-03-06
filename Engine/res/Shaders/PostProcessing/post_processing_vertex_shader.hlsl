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
    output.Position = float4(input.Position, 1.0f);
    output.TextureCoord = (input.Position.xy + 1) / 2;
    output.TextureCoord.y = 1 - output.TextureCoord.y;

    return output;
}