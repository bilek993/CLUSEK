struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = float4(input.Position, 1.0f);
    output.Color = input.Color;

    return output;
}